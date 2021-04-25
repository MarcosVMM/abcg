#include "ship.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <stdio.h>

void Ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // clang-format off
  std::array<glm::vec2, 37> positions{

      // Ship body
      glm::vec2{0.0f, +20.0f}, glm::vec2{-7.0f, +11.0f},
      glm::vec2{0.0f, +14.0f}, glm::vec2{7.0f, 11.0f},
      glm::vec2{-14.0f, 2.0f}, glm::vec2{0.0f, 7.5f}, glm::vec2{14.0f, 2.0f},
      glm::vec2{-21.0f, -2.0f}, glm::vec2{+21.0f, -2.0f},
      

      // Cannon left 1
      glm::vec2{-16.5f, 2.0f}, glm::vec2{-16.5f, +5.0f},
      glm::vec2{-14.0f, 2.0f}, glm::vec2{-14.0f, +05.0f},

      // Cannon Left 2
      glm::vec2{-21.0f, -2.0f}, glm::vec2{-21.0f, 2.0f},
      glm::vec2{-18.0f, -2.0f}, glm::vec2{-18.0f, 2.0f},

      // Cannon Right 1
      glm::vec2{16.5f, 02.0f}, glm::vec2{16.5f, +05.0f},
      glm::vec2{14.0f, 02.0f}, glm::vec2{14.0f, +05.0f},

      // Cannon Right 2
      glm::vec2{21.0f, -2.0f}, glm::vec2{21.0f, 2.0f},
      glm::vec2{18.0f, -2.0f}, glm::vec2{18.0f, 2.0f},
      
      //Propulsor left
      glm::vec2{-10.0f, -5.0f}, 
      glm::vec2{-7.0f, 0.0f}, 
      glm::vec2{-4.0f, -5.0f},

    //Propulsor Right
      glm::vec2{10.0f, -5.0f}, 
      glm::vec2{7.0f, 0.0f}, 
      glm::vec2{4.0f, -5.0f},

      // Thruster trail (left)
      glm::vec2{-10.0f, -5.0f}, 
      glm::vec2{-7.0f, -14.0f}, 
      glm::vec2{-4.0f, -5.0f},

      // Thruster trail (right)
      glm::vec2{10.0f, -5.0f}, 
      glm::vec2{7.0f, -14.0f}, 
      glm::vec2{4.0f, -5.0f},

      };

  // Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 2,
                     0, 2, 3,
                     2, 4, 5,
                     2, 5, 6,
                     5, 7, 8,
                   
                     // Cannons
                     9, 10, 11,
                     10, 11, 12,
                     13, 14, 15,
                     14, 15, 16,
                     17, 18, 19,
                     18, 19, 20,
                     21, 22, 23,
                     22, 23, 24,

                    //Propulsores
                      25, 26, 27,
                     28, 29, 30,
                     
                     // Thruster trails
                     31, 32, 33,
                     34, 35, 36
                     };
  // clang-format on

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Ship::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform1f(m_rotationLoc, m_rotation);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      glUniform4f(m_colorLoc, 1, 0.5, 0.5, 0.5f);

      glDrawElements(GL_TRIANGLES, 17 * 3, GL_UNSIGNED_INT, nullptr);

      glDisable(GL_BLEND);
    }
  }

  // Color ship
  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 15 * 3, GL_UNSIGNED_INT, nullptr);

  glUniform4f(m_colorLoc, 1, 0.5, 0.5, 1.0f);
  glDrawElements(GL_TRIANGLES, 5 * 3, GL_UNSIGNED_INT, nullptr);
  
  glUniform4f(m_colorLoc, 0.5, 0.5, 0.5, 1.0f);
  glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, nullptr);

  glUniform4f(m_colorLoc, 1, 1, 0.5, 1.0f);
  glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);
 

  glBindVertexArray(0);

  glUseProgram(0);
}

void Ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData, float deltaTime) {
  // Rotate
  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

  // Apply thrust
  if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    float fator= (fase_atual+0.5)/2.0;
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
    m_velocity += forward * deltaTime * fator;
   
  }
}
