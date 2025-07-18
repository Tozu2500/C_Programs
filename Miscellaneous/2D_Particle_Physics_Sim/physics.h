#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>

#define MAX_PARTICLES 50
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRAVITY 0.5f
#define DAMPING 0.98f
#define PARTICLE_RADIUS 10.0f

typedef struct {
    float x, y;
} Vector2;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    float mass;
    bool active;
    int r, g, b;
} Particle;

typedef struct {
    Particle particles[MAX_PARTICLES];
    int particle_count;
    float dt;
} PhysicsSystem;

void physics_init(PhysicsSystem* system);
void physics_update(PhysicsSystem* system, float delta_time);
void physics_add_particle(PhysicsSystem* system, float x, float y, float vx, float vy);
void physics_handle_collision(PhysicsSystem* system);
void physics_apply_forces(PhysicsSystem* system);
void physics_integrate(PhysicsSystem* system);

Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_sub(Vector2 a, Vector2 b);
Vector2 vector2_scale(Vector2 v, float scalar);
float vector2_length(Vector2 v);
float vector2_distance(Vector2 a, Vector2 b);
Vector2 vector2_normalize(Vector2 v);

#endif // PHYSICS_H
