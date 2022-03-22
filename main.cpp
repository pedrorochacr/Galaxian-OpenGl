/*

Nome: Pedro Henrique Rocha de Castro
*/




#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define quantidade 10//quantidade de inimigos

struct jogo{//estrutura jogo para realizar funcao de pause
bool pause=false;
int auxInimigosMortos=0;//auxiliar que conta a quantidade de inimigos mortos
};

struct velocidade{
    float x=2;//x=2, velocidade do inimigo no eixo x
    float y;//y para as velocidades do tiro do personagem e do inimigo
};

struct personagem{
    float x, y;
    float comp, altura;
    bool vivo;
};


struct personagem principal;//principal= nave a ser utilizada para jogar
struct personagem inimigo[quantidade];
struct personagem tiro;//tiro é definido como personagem, pois possui coordenadas x,y,comp, altura.
struct personagem tiroInimigo[quantidade];
struct jogo jogo;




struct velocidade velInimigo;
struct velocidade velTiro;//tiro personagem e inimigo


GLuint explosao;
GLuint idCenario;
GLuint inimigos;
GLuint nave;
GLuint ataqueInimigo;
GLuint tiroPerson;
GLuint carregaTextura(const char* arquivo) {//funcao para carregar texturas
    GLuint idTextura = SOIL_load_OGL_texture(
                           arquivo,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );

    return idTextura;
}

bool colisao(float Ax, float Ay, float Acomp, float Aalt,float Bx, float By, float Bcomp, float Balt){
//funcao para detectar todas as colisoes do jogo
    if(Ay+Aalt<By){
        return false;
    }
    if(Ay> By+ Balt){
            return false;
    }

    if(Ax+(Acomp)<Bx){
            return false;
    }

    if((Ax) > Bx+ (Bcomp)){
            return false;
    }

    return true;

}
void colisaoInimigo(){
    //funcao que detecta a colisao do  tiro do personagem com os inimigos
    for(int i=0; i<quantidade;i++){
        if(inimigo[i].vivo){
            if(colisao(tiro.x, tiro.y, tiro.comp, tiro.altura,inimigo[i].x,
                       inimigo[i].y, inimigo[i].comp, inimigo[i].altura)== true){
                inimigo[i].vivo= false;
                tiro.vivo=false;//tiro.vivo= false para sumir da tela e nao atravessar os inimigos
                jogo.auxInimigosMortos++;//soma 1 pois um inimigo morre
                break;

            }
        }

    }

}


void criaTiroPersonagem(){

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, tiroPerson);
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(tiro.x, tiro.y, 0);

        glTexCoord2f(1.0, 0.0);
        glVertex3f(tiro.x + tiro.comp, tiro.y, 0);

        glTexCoord2f(1.0, 1.0);
        glVertex3f(tiro.x + tiro.comp, tiro.y + tiro.altura , 0);

        glTexCoord2f(0.0, 1.0);
        glVertex3f(tiro.x, tiro.y + tiro.altura , 0);



    glEnd();

    glDisable(GL_TEXTURE_2D);


}
void inicializa() {
//inicializa algumas funções e variaveis do sistema
    glClearColor(0, 0, 0, 0); //define a cor de fundo como preto

    principal.x=300;
    principal.y=350;
    principal.comp=30;
    principal.altura=30;
    principal.vivo=true;//nave começa vivo

    tiro.x=  principal.x;//coloca o tiro na poiscao da nave
    tiro.y=  principal.y;
    tiro.comp=10;
    tiro.altura=5;
    tiro.vivo= false;
    velTiro.y=5;//velocidade do tiro da nave como 5

    jogo.auxInimigosMortos=0;

    glEnable(GL_BLEND );
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);


    explosao=carregaTextura("explosao.png");
    idCenario= carregaTextura("cenario.png");
    ataqueInimigo= carregaTextura("tiroInimigo.png");
    tiroPerson= carregaTextura("tiro.png");
    nave= carregaTextura("personagem.png");
    inimigos = carregaTextura("inimigo.png");

}


void inicializaInimigos(){
//funcao para inicializar as posicoes dos inimigos e dos tiros deles

    for(int i=0, x=100, y= 15;i<quantidade;i++, x+= 80){

        if(x> 450){//se passar de 450, muda para a proxima linha
            x=100;
            y += 33;
        }

        inimigo[i].x =x;
        inimigo[i].y= y;
        inimigo[i].comp= 30;
        inimigo[i].altura=15;
        inimigo[i].vivo= true;

        tiroInimigo[i].x=inimigo[i].x;//cria os tiros nas posicoes dos inimigos
        tiroInimigo[i].y=inimigo[i].y;
        tiroInimigo[i].comp=10;
        tiroInimigo[i].altura=5;
        tiroInimigo[i].vivo=true;//true, pois os ataques inimigos sao continuos
    }

}

void escreve(void* fonte, char texto[20] , float x, float y) {//funcao para escrever textos na tela
    glRasterPos2f(x, y);
    int tam=strlen(texto);
    for (int i=0; i <tam; i++) {
        glutBitmapCharacter(fonte, texto[i]);

  }

}

void GameOver(){
       //funcao utilizada para quando o personagem morre
        jogo.pause=true;//pausa o jogo, pois acabou, a partir disso o usuario decide se quer reiniciar ou sair
         glColor3f (1.0, 0.0, 0.0);
         escreve(GLUT_BITMAP_HELVETICA_18, "Game Over", 250, 200);


}
void criaExplosaoNave(){
    //funcao utilizada para criar a explosao da nave
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, explosao);//começa a usar a textura
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
        if(principal.vivo==false){//nave foi atingida
            glTexCoord2f(0.0, 0.0);
            glVertex3f(principal.x, principal.y, 0);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(principal.x + 60, principal.y, 0);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(principal.x + 60, principal.y + principal.altura , 0);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(principal.x, principal.y + principal.altura, 0);
        }
    glEnd();

    glDisable(GL_TEXTURE_2D);

}


void desenhaMinhaCena() {

    glClear(GL_COLOR_BUFFER_BIT);


   glEnable(GL_TEXTURE_2D);



   glBindTexture(GL_TEXTURE_2D, idCenario);//cria a textura do cenario ligado aos quatro cantos da tela
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(0, 0, 0);

        glTexCoord2f(1.0, 0.0);
        glVertex3f(600, 0, 0);

        glTexCoord2f(1.0, 1.0);
       glVertex3f(600, 400 , 0);

        glTexCoord2f(0.0, 1.0);
       glVertex3f(0, 400 , 0);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, nave);//cria a nave
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
        if(principal.vivo){
            glTexCoord2f(0.0, 0.0);
            glVertex3f(principal.x, principal.y, 0);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(principal.x + principal.comp, principal.y, 0);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(principal.x + principal.comp,principal.y + principal.altura , 0);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(principal.x, principal.y + principal.altura, 0);
        }
    glEnd();

    glDisable(GL_TEXTURE_2D);


    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, inimigos);//cria os inimigos
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        for(int i=0; i<quantidade; i++){
            if(inimigo[i].vivo){//so cria o inimigo se ele estiver vivo
                glTexCoord2f(0.0, 0.0);
                glVertex3f(inimigo[i].x, inimigo[i].y, 0);

                glTexCoord2f(1.0, 0.0);
                glVertex3f(inimigo[i].x + inimigo[i].comp, inimigo[i].y, 0);

                glTexCoord2f(1.0, 1.0);
                 glVertex3f(inimigo[i].x + inimigo[i].comp, inimigo[i].y + inimigo[i].altura , 0);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(inimigo[i].x, inimigo[i].y + inimigo[i].altura , 0);

             }

        }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, ataqueInimigo);//cria os tiros dos inimigos
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);


           for(int i=0; i<quantidade; i++){
            if(inimigo[i].vivo){//so cria o tiro inimigo se ele estiver vivo
                if(tiroInimigo[i].vivo){//tiro do inimigo tem que estar acionado
                    glTexCoord2f(0.0, 0.0);
                    glVertex3f(tiroInimigo[i].x, tiroInimigo[i].y, 0);

                    glTexCoord2f(1.0, 0.0);
                    glVertex3f(tiroInimigo[i].x +tiroInimigo[i].comp, tiroInimigo[i].y, 0);

                    glTexCoord2f(1.0, 1.0);
                    glVertex3f(tiroInimigo[i].x + tiroInimigo[i].comp, tiroInimigo[i].y + tiroInimigo[i].altura , 0);

                    glTexCoord2f(0.0, 1.0);
                    glVertex3f(tiroInimigo[i].x,tiroInimigo[i].y + tiroInimigo[i].altura , 0);
                }




             }

        }
    glEnd();



    glDisable(GL_TEXTURE_2D);

    if(tiro.vivo){//tiro.vivo=true, tiro personagem acionado
        criaTiroPersonagem();//chama funcao pra criar o tiro do personagem
    }
    if(principal.vivo==false){//personagem morreu ou inimigo atravessou a tela ou encostou na nave
        criaExplosaoNave();
        GameOver();

    }

    if((jogo.pause)&&(principal.vivo)){//jogo pausado e personagem vivo quer dizer que o usuario apertou p
        glColor3f (1.0, 0.0, 0.0);
        escreve(GLUT_BITMAP_TIMES_ROMAN_24, "Jogo Pausado", 250, 200);

    }

    if(jogo.auxInimigosMortos==10){//jogo encerra, pois morreram todos inimigos
        glColor3f (1.0, 0.0, 0.0);
        escreve(GLUT_BITMAP_HELVETICA_18, "Parabens!! Voce Venceu!!", 250, 200);
    }

    glutSwapBuffers();

}

void redimensionada(int width, int height) {
   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 600, 400, 0, -1, 1);//define a area de desenho na tela

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void movimentoTiroInimigo(){
//movimenta os tiros inimigos
    for(int i=0; i<quantidade;i++){
        if(inimigo[i].vivo){
            tiroInimigo[i].y=tiroInimigo[i].y+velTiro.y;//aumenta a coordenada y dos tiros inimigos

            if(tiroInimigo[i].y>400){//se atravessou a tela, volta para a posicao inicial
                tiroInimigo[i].y= inimigo[i].y;
                tiroInimigo[i].x=inimigo[i].x;
            }
            if(colisao(principal.x,principal.y, principal.comp, principal.altura,tiroInimigo[i].x, tiroInimigo[i].y,
                    tiroInimigo[i].comp, tiroInimigo[i].altura)==true){//tiro inimigo acertou o personagem
                    principal.vivo=false;
                    tiroInimigo[i].vivo= false;
                    break;
            }
        }
    }
}


void movimentoTiro(){
   //movimenta os tiros do personagem

   tiro.y = tiro.y -velTiro.y;//movimenta ao longo do eixoY
    if(tiro.vivo){
        tiro.y = tiro.y -velTiro.y;
        if(tiro.y< 0){
            tiro.vivo=false;
        }

    }
    if(!tiro.vivo){//tiro.vivo= false. Com isso, volta para a posicao da nave
        tiro.y = tiro.y -velTiro.y;
        tiro.y=principal.y;
        tiro.x= principal.x;

    }
    if(jogo.pause){
        velTiro.y=0;//nao movimenta os tiros dos inimigos e do personagem

    }
    if(jogo.pause== false){
        velTiro.y=5;//volta a movimentar
    }
}

void movimentoInimigo(struct personagem inimigo[], struct personagem tiroInimigo[]){
//funcao que realiza os movimentos dos inimigos
    if(jogo.pause== false){
        for(int i=0; i<quantidade; i++){
            inimigo[i].x=inimigo[i].x + velInimigo.x;

            if(inimigo[i].x <0){
                for(int j=0; j<quantidade; j++){//quando bate na parte esquerda da tela, todos os inimigos aumentam 15 na coordenada y
                    inimigo[j].y=inimigo[j].y+15;

                }
                velInimigo.x=-velInimigo.x;//velInimigo negativo para fazer movimento contrario
            }
            if(inimigo[i].x+inimigo[i].comp>600){//encosta na parte direita da tela

                for(int j=0; j<quantidade; j++){
                    inimigo[j].y=inimigo[j].y+15;
                }
                velInimigo.x=-velInimigo.x;
            }
            if(inimigo[i].y+ inimigo[i].altura>400){//quando os inimigos atravessam o cenario
                principal.vivo=false;//nave morre, pois deu GameOver
                break;
            }

            if(colisao(inimigo[i].x,inimigo[i].y,inimigo[i].comp,inimigo[i].comp,principal.x,principal.y,principal.comp,principal.altura)== true){//inimigos encostaram na nave
                principal.vivo=false;//nave morre, pois deu GameOver
                break;
            }

        }
    }
    if(jogo.pause){}
        //jogo pausado, nao movimenta os inimigos
}


void atualizaCena(int periodo){
//funcao para atualizar a cena de acordo com os acontecimentos
    movimentoInimigo(inimigo, tiroInimigo);
    colisaoInimigo();

    movimentoTiro();
    movimentoTiroInimigo();


    glutPostRedisplay();//redesenha
    glutTimerFunc(periodo,atualizaCena, periodo);
}

void teclaPressionada(unsigned char key, int x, int y) {



    switch(key) {
    case 27:// esc
        exit(0);  // Sai da aplicação
        break;
    case 97:// a
        principal.x= principal.x-20;// movimenta a nave
         if(principal.x<20){
            principal.x=20;//nao atravessa a tela
        }


        break;
    case 100:// d

        principal.x= principal.x +20;
        if(principal.x+principal.comp>580){
            principal.x=580- principal.comp;// nao atravessa a tela
        }
        break;
    case 32://espaco

        tiro.vivo= true;// habilita o tiro
        break;
    case 112://p
        jogo.pause=!jogo.pause;//inverte o valor do pause, para pausar e despausar
        break;

    case 114://r
        jogo.pause= false;
        inicializa();//reinicia o jogo nas posicoes iniciais
        inicializaInimigos();
        break;

    default:
        break;
    }

     glutPostRedisplay();

}
int main(int argc, char** argv) {

    inicializaInimigos();
   glutInit(&argc, argv);

   glutInitContextVersion(1, 1);
   glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

   glutInitDisplayMode(GLUT_RGB| GLUT_DOUBLE);
   glutInitWindowSize(600, 600);
   glutInitWindowPosition(100, 100);

   glutCreateWindow("GALAXIAN");

   inicializa();

   glutDisplayFunc(desenhaMinhaCena);

   glutReshapeFunc(redimensionada);
   glutKeyboardFunc(teclaPressionada);
   glutTimerFunc(0, atualizaCena, 33);



   glutMainLoop();
   return 0;
}

