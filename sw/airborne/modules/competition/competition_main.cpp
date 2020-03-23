#include "competition_main_cxx.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define NAV_C

#include "competition_main.h"
#include "firmwares/rotorcraft/guidance/guidance_h.h"
#include "generated/airframe.h"
#include "generated/flight_plan.h"
#include "modules/competition/opticflow/opticflow.h"
#include "state.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define STATE_WAIT_HEADING 0
#define STATE_CONTINUE 1
#define STATE_OUT_OF_OBSTACLE_ZONE 2

#define HEADING_M 0
#define HEADING_RATE_M 1

int current_state = STATE_CONTINUE;

int hold = 0;

float _headingRate = 0;
float _speed = 0;
float _heading = 0;

int mode = HEADING_M;

void competition_init() { 
    CompetitionInit(); 
    guidance_h_SetMaxSpeed(10.0f);
    guidance_h.gains.v = 100.;
}

uint8_t tick = 0;

void competition_loop() {
  CompetitionLoop();

  NedCoor_f *ned = stateGetPositionNed_f();
  NedCoor_f *accel = stateGetAccelNed_f();
  struct FloatEulers *eulers = stateGetNedToBodyEulers_f();
  FloatRates *rates = stateGetBodyRates_f();

  if (!InsideObstacleZone(ned->x, ned->y)) {
    current_state = STATE_OUT_OF_OBSTACLE_ZONE;
  } else if (current_state == STATE_OUT_OF_OBSTACLE_ZONE) {
    current_state = STATE_CONTINUE;
  }

  switch (current_state) {
  case STATE_OUT_OF_OBSTACLE_ZONE: {
    float psi = atan2(ned->y, ned->x) + M_PI;

    while (psi > 2 * M_PI) {
      psi -= 2 * M_PI;
    }
    while (psi < 0) {
      psi += 2 * M_PI;
    }

    setHeading(psi);
    setSpeed(1.);
  } break;
  case STATE_CONTINUE: {
    // Opticflow (only check when accel < 1e-2)
    float accelY = ACCEL_FLOAT_OF_BFP(accel->y);
    float accelZ = ACCEL_FLOAT_OF_BFP(accel->z);
    if (sqrt(pow(rates->p, 2) + pow(rates->q, 2) + pow(rates->r, 2)) < .1 &&
        eulers->phi < 1e-1 && accelY < .1 && accelZ < .1) {
      switch (of_get_suggested_action()) {
      case OF_ACT_STRAIGHT: {
        setSpeed(1.);
        setHeadingRate(0.);
      } break;
      case OF_ACT_LEFT: {
        fprintf(stderr, "[OF_ACT] Go left\n");
        setHeadingRate(-30 * M_PI / 180.);
        setSpeed(0);
        hold = 30;
        current_state = STATE_WAIT_HEADING;
      } break;
      case OF_ACT_RIGHT: {
        fprintf(stderr, "[OF_ACT] Go right\n");
        setHeadingRate(30 * M_PI / 180.);
        setSpeed(0);
        hold = 30;
        current_state = STATE_WAIT_HEADING;
      } break;
      }
    } else {
        fprintf(stderr, "[OF] N/A %d\n", tick++);
    }
  } break;
  case STATE_WAIT_HEADING: {
    setSpeed(0);
    if (hold > 0) {
      --hold;
    } else {
      setHeadingRate(0.0f);
      current_state = STATE_CONTINUE;
    }
  } break;
  }

  switch (mode) {
  case HEADING_M: {
    guidance_h_set_guided_heading(_heading);
  } break;
  case HEADING_RATE_M:
  default: {
    guidance_h_set_guided_heading_rate(_headingRate);
  } break;
  }

  guidance_h_set_guided_vel(_speed * cos(eulers->psi),
                            _speed * sin(eulers->psi));
}

void setHeadingRate(float headingRate) {
  _headingRate = headingRate;
  mode = HEADING_RATE_M;
}

void setHeading(float heading) {
  _heading = heading;
  mode = HEADING_M;
}

void setSpeed(float speed) { _speed = speed; }

#ifdef __cplusplus
}
#endif
