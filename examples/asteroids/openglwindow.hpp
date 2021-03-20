#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <random>

#include "abcg.hpp"
#include "asteroids.hpp"
#include "bullets.hpp"
#include "ship.hpp"
#include "starlayers.hpp"
#include "user.hpp"

class user;

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_starsProgram{};
  GLuint m_objectsProgram{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  float velocidade_x;
  float velocidade_y;
  int velocidade;
  float fator_screencolor;
  bool restarting;
  GameData m_gameData;
  GameData m_gameData_aux;

  Asteroids m_asteroids;
  Bullets m_bullets;
  Ship m_ship;
  StarLayers m_starLayers;
  
  abcg::ElapsedTimer m_restartWaitTimer;

  ImFont* m_font{};
  ImFont* m_font2{};

  std::default_random_engine m_randomEngine;

  void checkCollisions();
  void checkWinCondition();

  void restart();
  void update();

  void refreshFase(int fase_atual);

  public:
  user usuario;


};
#endif