#ifndef MOVEMENT_H
#define MOVEMENT_H

int closer_to_zero(int n);

int cap_speed_and_apply_friction(int n, int accel_friction, int max_speed);

void evaluate_movement();

#endif  /* !MOVEMENT_H */