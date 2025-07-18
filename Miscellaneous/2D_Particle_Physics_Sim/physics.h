#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>

// Constants
#define MAX_PARTICLES 50
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 30
#define GRAVITY 0.5f
#define DAMPING 0.98f
#define PARTICLE_RADIUS 2.0f

// Vector structure
typedef struct {
    float x, y;
} Vector2;

// Particle structure
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    float mass;
    bool active;
    // Color components (RGB)
    int r, g, b;
} Particle;

// Physics system structure
typedef struct {
    Particle particles[MAX_PARTICLES];
    int particle_count;
    float dt; // Delta time
} PhysicsSystem;

// Function declarations
void physics_init(PhysicsSystem* system);
void physics_update(PhysicsSystem* system, float delta_time);
void physics_add_particle(PhysicsSystem* system, float x, float y, float vx, float vy);
void physics_handle_collisions(PhysicsSystem* system);
void physics_apply_forces(PhysicsSystem* system);
void physics_integrate(PhysicsSystem* system);

// Vector math functions
Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_sub(Vector2 a, Vector2 b);
Vector2 vector2_scale(Vector2 v, float scalar);
float vector2_length(Vector2 v);
float vector2_distance(Vector2 a, Vector2 b);
Vector2 vector2_normalize(Vector2 v);

#endif // PHYSICS_H
