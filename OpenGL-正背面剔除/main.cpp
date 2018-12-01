//
//  main.cpp
//  OpenGL-正背面剔除
//
//  Created by lxj on 2018/12/1.
//  Copyright © 2018 lxj. All rights reserved.
//

#include <stdio.h>

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

////设置角色帧，作为相机
GLFrame             viewFrame;
//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

//右键菜单栏选项
void ProcessMenu(int value)
{
    switch(value)
    {
        case 1:
            iDepth = !iDepth;
            break;
            
        case 2:
            iCull = !iCull;
            break;
            
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
            
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
            
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
    }
    
    glutPostRedisplay();
}

//渲染
void renderScreen(void){
    //清空缓冲区数据
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //开启背面剔除
    if (iCull){
        
        glEnable(GL_CULL_FACE);
        //制定逆时针三角形为正三角形,即为逆时针为正面,默认逆时针s面为正面，可不写
        glFrontFace(GL_CCW);
        
        //开启背面剔除
        glCullFace(GL_BACK);
    }else{
        
        glDisable(GL_CULL_FACE);
    }
    
    //根据设置iDepth标记来判断是否开启深度测试
    if(iDepth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    //把摄像机矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(viewFrame);
    
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    
    //使用平面着色器
    //参数1：平面着色器
    //参数2：模型视图投影矩阵
    //参数3：颜色
    //shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    
    //使用默认光源着色器
    //通过光源、阴影效果跟提现立体效果
    //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
    //参数2：模型视图矩阵
    //参数3：投影矩阵
    //参数4：基本颜色值
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
    
    torusBatch.Draw();
    
    //出栈
    
    modelViewMatix.PopMatrix();
    
    glutSwapBuffers();
    
}



//默认设置
void setup(){
    
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f );

    shaderManager.InitializeStockShaders();
    
    //窗口与相机的距离
    viewFrame.MoveForward(7.0);
    
    //创建一个甜甜圈
    //void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    //参数1：GLTriangleBatch 容器帮助类
    //参数2：外边缘半径
    //参数3：内边缘半径
    //参数4、5：主半径和从半径的细分单元数量
    
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);

    //点的大小
    glPointSize(4.0f);
}

//窗口大小改变
void changeSize(int w, int h){
    
    //窗口尺寸
    glViewport(0, 0, w, h);
    
    // 设置透视模式，初始化其透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);

    //把透视矩阵加载到透视矩阵对阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
    
}

//点击方向键
void specialKey(int key, int x, int y){
    
    //改变世界坐标系，是图形移动
    
    if(key == GLUT_KEY_UP)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    //重新刷新window
    glutPostRedisplay();
    
}




int main(int argc, char* argv[]){
    
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGBA|GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    glutCreateWindow("正背面剔除");
    
    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScreen);
    glutSpecialFunc(specialKey);
    
    // Create the Menu
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Toggle depth test",1);
    glutAddMenuEntry("Toggle cull backface",2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        printf("出错");
        return 1;
    }
    
    setup();
    glutMainLoop();
    return 0;
}
