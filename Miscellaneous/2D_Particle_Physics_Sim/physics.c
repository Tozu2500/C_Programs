#include "physics.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Initialize physics system
void physics_init(PhysicsSystem* system) {
    memset(system, 0, sizeof(PhysicsSystem));
    system->particle_count = 0;
    system->dt = 0.016f; // ~60 FPS
}

// Update physics simulation
void physics_update(PhysicsSystem* system, float delta_time) {
    system->dt = delta_time;
    
    physics_apply_forces(system);
    physics_integrate(system);
    physics_handle_collisions(system);
}

// Add a new particle to the system
void physics_add_particle(PhysicsSystem* system, float x, float y, float vx, float vy) {
    if (system->particle_count >= MAX_PARTICLES) {
        return; // System is full
    }
    
    Particle* p = &system->particles[system->particle_count];
    p->position.x = x;
    p->position.y = y;
    p->velocity.x = vx;
    p->velocity.y = vy;
    p->acceleration.x = 0.0f;
    p->acceleration.y = 0.0f;
    p->radius = PARTICLE_RADIUS;
    p->mass = 1.0f;
    p->active = true;
    
    // Random colors
    p->r = rand() % 256;
    p->g = rand() % 256;
    p->b = rand() % 256;
    
    system->particle_count++;
}

// Apply forces to all particles
void physics_apply_forces(PhysicsSystem* system) {
    for (int i = 0; i < system->particle_count; i++) {
        Particle* p = &system->particles[i];
        if (!p->active) continue;
        
        // Reset acceleration
        p->acceleration.x = 0.0f;
        p->acceleration.y = 0.0f;
        
        // Apply gravity
        p->acceleration.y += GRAVITY;
    }
}

// Integrate motion using Euler integration
void physics_integrate(PhysicsSystem* system) {
    for (int i = 0; i < system->particle_count; i++) {
        Particle* p = &system->particles[i];
        if (!p->active) continue;
        
        // Update velocity with acceleration
        p->velocity.x += p->acceleration.x * system->dt;
        p->velocity.y += p->acceleration.y * system->dt;
        
        // Apply damping
        p->velocity.x *= DAMPING;
        p->velocity.y *= DAMPING;
        
        // Update position with velocity
        p->position.x += p->velocity.x * system->dt;
        p->position.y += p->velocity.y * system->dt;
    }
}

// Handle collisions with boundaries and between particles
void physics_handle_collisions(PhysicsSystem* system) {
    for (int i = 0; i < system->particle_count; i++) {
        Particle* p = &system->particles[i];
        if (!p->active) continue;
        
        // Boundary collisions
        if (p->position.x - p->radius < 0) {
            p->position.x = p->radius;
            p->velocity.x = -p->velocity.x * 0.8f; // Energy loss
        }
        if (p->position.x + p->radius > SCREEN_WIDTH) {
            p->position.x = SCREEN_WIDTH - p->radius;
            p->velocity.x = -p->velocity.x * 0.8f;
        }
        if (p->position.y - p->radius < 0) {
            p->position.y = p->radius;
            p->velocity.y = -p->velocity.y * 0.8f;
        }
        if (p->position.y + p->radius > SCREEN_HEIGHT) {
            p->position.y = SCREEN_HEIGHT - p->radius;
            p->velocity.y = -p->velocity.y * 0.8f;
        }
        
        // Particle-to-particle collisions
        for (int j = i + 1; j < system->particle_count; j++) {
            Particle* other = &system->particles[j];
            if (!other->active) continue;
            
            float distance = vector2_distance(p->position, other->position);
            float min_distance = p->radius + other->radius;
            
            if (distance < min_distance) {
                // Calculate collision normal
                Vector2 normal = vector2_normalize(vector2_sub(other->position, p->position));
                
                // Separate particles
                float overlap = min_distance - distance;
                Vector2 separation = vector2_scale(normal, overlap * 0.5f);
                
                p->position = vector2_sub(p->position, separation);
                other->position = vector2_add(other->position, separation);
                
                // Simple elastic collision response
                Vector2 relative_velocity = vector2_sub(p->velocity, other->velocity);
                float velocity_along_normal = relative_velocity.x * normal.x + relative_velocity.y * normal.y;
                
                if (velocity_along_normal > 0) continue; // Objects separating
                
                float restitution = 0.8f;
                float impulse = -(1 + restitution) * velocity_along_normal;
                impulse /= (1.0f / p->mass + 1.0f / other->mass);
                
                Vector2 impulse_vector = vector2_scale(normal, impulse);
                p->velocity = vector2_add(p->velocity, vector2_scale(impulse_vector, 1.0f / p->mass));
                other->velocity = vector2_sub(other->velocity, vector2_scale(impulse_vector, 1.0f / other->mass));
            }
        }
    }
}

// Vector math functions
Vector2 vector2_add(Vector2 a, Vector2 b) {
    Vector2 result = {a.x + b.x, a.y + b.y};
    return result;
}

Vector2 vector2_sub(Vector2 a, Vector2 b) {
    Vector2 result = {a.x - b.x, a.y - b.y};
    return result;
}

Vector2 vector2_scale(Vector2 v, float scalar) {
    Vector2 result = {v.x * scalar, v.y * scalar};
    return result;
}

float vector2_length(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

float vector2_distance(Vector2 a, Vector2 b) {
    return vector2_length(vector2_sub(a, b));
}

Vector2 vector2_normalize(Vector2 v) {
    float length = vector2_length(v);
    if (length > 0.0f) {
        return vector2_scale(v, 1.0f / length);
    }
    Vector2 zero = {0.0f, 0.0f};
    return zero;
}