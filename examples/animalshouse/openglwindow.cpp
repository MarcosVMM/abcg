#include "openglwindow.hpp"

#include <imgui.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "imfilebrowser.h"

void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mousePress(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mouseRelease(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0.1, 0.1, 0.1, 0.5);
  glEnable(GL_DEPTH_TEST);

  // Create programs
  for (const auto& name : m_shaderNames) {
    //const auto& name = "blinnphong";
    auto path{getAssetsPath() + "shaders/" + name};
    auto program{createProgramFromFile(path + ".vert", path + ".frag")};
    m_programs.push_back(program);
  }

  m_mappingMode = 0;
  // Initial trackball spin
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  
  m_trackBallModel.setVelocity(0.0001f);
  m_trackBallLight.setVelocity(0.0001f);

}
//Alterado
int aux_mtriangles;
bool start= true;
void OpenGLWindow::loadModel(std::string_view path,int random) {
  /*m_model.loadDiffuseTexture(getAssetsPath() + "maps/pattern.png");
  m_model.loadNormalTexture(getAssetsPath() + "maps/pattern_normal.png");*/
  //alterei
    start = false;
  if(random == 1){
    m_model.loadDiffuseTexture(getAssetsPath() + "maps/Bird.jpg");
    m_model.loadNormalTexture(getAssetsPath() + "maps/Bird_normal.jpg");
  }else if(random == 2){
    m_model.loadDiffuseTexture(getAssetsPath() + "maps/Cat_diffuse.jpg");
    m_model.loadNormalTexture(getAssetsPath() + "maps/Cat_normal.jpg");
  }else if(random == 3){
    m_model.loadDiffuseTexture(getAssetsPath() + "maps/Horse.jpg");
    m_model.loadNormalTexture(getAssetsPath() + "maps/Horse_normal.png");
  }else if(random == 4){
    m_model.loadDiffuseTexture(getAssetsPath() + "maps/Kangaroo.jpg");
    m_model.loadNormalTexture(getAssetsPath() + "maps/KangarooKangaroo_normal.png");
  }else if(random == 5){
    m_model.loadDiffuseTexture(getAssetsPath() + "maps/Monkey.jpg");
    m_model.loadNormalTexture(getAssetsPath() + "maps/Monkey_normal.png");
  }

   m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));

  m_model.loadFromFile(path);
  m_model.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw = m_model.getNumTriangles();
  aux_mtriangles = 0; 
  // Use material properties from the loaded model
 // m_Ka = m_model.getKa();
 // m_Kd = m_model.getKd();
 // m_Ks = m_model.getKs();
      m_Ia.x = 0.017;
      m_Ia.y = 0.017;
      m_Ia.z = 0.017;

      m_Id.x = 0.34;
      m_Id.y = 0.34;
      m_Id.z = 0.34;

      m_Is.x = 0;
      m_Is.y = 0;
      m_Is.z = 0;

      m_Ka.x = 0.6;
      m_Ka.y = 0.6;
      m_Ka.z = 0.6;

      m_Kd.x = 0.6;
      m_Kd.y = 0.6;
      m_Kd.z = 0.6;

      m_Ks.x = 0.6;
      m_Ks.y = 0.6;
      m_Ks.z = 0.6;


  m_shininess = m_model.getShininess();
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  m_trackBallModel.setVelocity(0.0008f);
}
float fator=0.4;
float fator2 = -0.3;
float aux_fator = fator;
float aux_fator2 = fator2;
void OpenGLWindow::paintGL() {

  if(fator>=aux_fator || fator >= aux_fator2){
    fator = fator - 0.001;
    update(fator);
  }
  else if(fator2 <=aux_fator2 || fator2 <= aux_fator){
    fator2 = fator2 + 0.001;
    update(fator2);
  }
  else{
    fator = aux_fator;
    fator2 = aux_fator2;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  glUseProgram(program);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(mappingModeLoc, m_mappingMode);

  auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);
  
  if (aux_mtriangles < m_trianglesToDraw){
    m_model.render(aux_mtriangles);
    aux_mtriangles=aux_mtriangles + (int) m_trianglesToDraw/5000;
  }
  else{
     m_model.render(m_trianglesToDraw);
     float color = 0.7;
     m_Ia.x =color;
      m_Ia.y =  color;
      m_Ia.z =  color;

      m_Id.x = color;
      m_Id.y =color;
      m_Id.z = color;

      m_Is.x = color;
      m_Is.y = color;
      m_Is.z = color;
  }
  

  glUseProgram(0);
}


void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  
  if (start == true){
   auto size{ImVec2(300, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};

    ImGui::Begin(" ", nullptr, flags);
    ImGui::Text("Choose your favorite animal!");
    ImGui::End();
  }
   



// Only in WebGL
/*#if defined(__EMSCRIPTEN__)
  fileDialogModel.SetPwd(getAssetsPath());
  fileDialogDiffuseMap.SetPwd(getAssetsPath() + "/maps");
  fileDialogNormalMap.SetPwd(getAssetsPath() + "/maps");
#endif*/

  // Create main window widget

  {

      static bool faceCulling{};
    if (faceCulling) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }

    
    // Projection combo box
   
    {
      static std::size_t currentIndex{};
     // std::vector<std::string> comboItems{"Perspective", "Orthographic"};
      currentIndex = 0;

      auto aspect{static_cast<float>(m_viewportWidth) /
                  static_cast<float>(m_viewportHeight)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

      } else {
        m_projMatrix =
            glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }
     
  }{

    auto size{ImVec2(120, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         20)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};

    ImGui::Begin(" ", nullptr, flags);
    ImGui::Text("Animal's House");
    ImGui::End();
  
  }
  auto size{ImVec2(170, 350)};
  auto position{ImVec2((m_viewportWidth - size.x),
                         0)};
    
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(position);

 ImGuiWindowFlags flags{
                           ImGuiWindowFlags_NoFocusOnAppearing |
                           ImGuiWindowFlags_NoBringToFrontOnFocus 
                           };
    
    //Alterado
    //Alternativas
static std::size_t currentIndex{};
  ImGui::Begin("Animals",nullptr, flags);
  ImGui::Text("Select!");
  ImGui::Columns(2, NULL, false);
  if(ImGui::Button("Bear", ImVec2(80, 50))){
      
        currentIndex = 2;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));

  float color = 0.6;
    m_Ia.x = color;
    m_Ia.y = color;
    m_Ia.z = color;

    m_Id.x = color;
    m_Id.y = color;
    m_Id.z = color;

    m_Is.x = color;
    m_Is.y = color;
    m_Is.z = color;
    
    loadModel((getAssetsPath() + "Bear.obj"),0);
    m_mappingMode = 0;
    
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  }
  if(ImGui::Button("Bird", ImVec2(80, 50))){
        currentIndex = 0;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
    loadModel((getAssetsPath() + "Bird.obj"),1);
    m_mappingMode = 3;
  }
  if(ImGui::Button("Cat", ImVec2(80, 50))){
        currentIndex = 0;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
    loadModel((getAssetsPath() + "catzinho.obj"),2);
    m_mappingMode = 3;
  }
  if(ImGui::Button("Dog", ImVec2(80, 50))){
        currentIndex = 2;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));

  float color = 0.6;
    m_Ia.x = color;
    m_Ia.y = color;
    m_Ia.z = color;

    m_Id.x = color;
    m_Id.y = color;
    m_Id.z = color;

    m_Is.x = color;
    m_Is.y = color;
    m_Is.z = color;
    
    loadModel((getAssetsPath() + "Dog.obj"),0);
    m_mappingMode = 0;
  }
  if(ImGui::Button("Elephant", ImVec2(80, 50))){
      
        currentIndex = 3;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));

  float color = 0.6;
    m_Ia.x = color;
    m_Ia.y = color;
    m_Ia.z = color;

    m_Id.x = color;
    m_Id.y = color;
    m_Id.z = color;

    m_Is.x = color;
    m_Is.y = color;
    m_Is.z = color;
    
    loadModel((getAssetsPath() + "Elephant.obj"),0);
    m_mappingMode = 0;
    
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
}

ImGui::NextColumn();

if(ImGui::Button("Horse", ImVec2(80, 50))){
        currentIndex = 0;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
    loadModel((getAssetsPath() + "Horse.obj"),3);
    m_mappingMode = 3;
}

if(ImGui::Button("Kangaroo", ImVec2(80, 50))){
        currentIndex = 0;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
    loadModel((getAssetsPath() + "Kangaroo.obj"),4);
    m_mappingMode = 3;
}

if(ImGui::Button("Monkey", ImVec2(80, 50))){
        currentIndex = 0;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
    loadModel((getAssetsPath() + "Monkey.obj"),5);
    m_mappingMode = 3;
}

if(ImGui::Button("Pig", ImVec2(80, 50))){
      
        currentIndex = 2;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));

  float color = 0.6;
    m_Ia.x = color;
    m_Ia.y = color;
    m_Ia.z = color;

    m_Id.x = color;
    m_Id.y = color;
    m_Id.z = color;

    m_Is.x = color;
    m_Is.y = color;
    m_Is.z = color;
    
    loadModel((getAssetsPath() + "Pig.obj"),0);
    m_mappingMode = 0;
    
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  }
    
  if(ImGui::Button("Rabbit", ImVec2(80, 50))){
      
        currentIndex = 2;
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));

  float color = 0.6;
    m_Ia.x = color;
    m_Ia.y = color;
    m_Ia.z = color;

    m_Id.x = color;
    m_Id.y = color;
    m_Id.z = color;

    m_Is.x = color;
    m_Is.y = color;
    m_Is.z = color;
    
    loadModel((getAssetsPath() + "Rabbit.obj"),0);
    m_mappingMode = 0;
    
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  }
    ImGui::End();
}
  

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBallModel.resizeViewport(width, height);
  m_trackBallLight.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  for (const auto& program : m_programs) {
    glDeleteProgram(program);
  }
}

void OpenGLWindow::update(float i) {
  m_modelMatrix = m_trackBallModel.getRotation();

  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f + m_zoom),
                  glm::vec3(0.0f, i, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                  
 
}