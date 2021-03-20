#include "user.hpp"
#include <iostream>
#include <stdio.h>

user::user(){}

void user::setVida(int x){
  this->vida=x;
}
int user::getVida(){
    return this->vida;
}
void user::setFase(int x){
    this->fase=x;
}
int user::getFase(){
    return this->fase;
}
void user::setWin(int x){
    this->win=x;
}
int user::getWin(){
    return this->win;
}
void user::setMaxVida(int x){
    this->max_vida=x;
}
int user::getMaxVida(){
    return this->max_vida;
}
void user::setMaxFase(int x){
    this->max_fase=x;
}
int user::getMaxFase(){
    return this->max_fase;
}
