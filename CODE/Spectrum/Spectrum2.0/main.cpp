//
//  main.cpp
//  Spectrum2.0
//
//  Created by boone on 2018/8/6.
//  Copyright © 2018年 boone. All rights reserved.
//

#define OLD_FILE_PATH "/Users/boone/Desktop/Music/Seve.pcm"     //PCM源文件

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

using namespace std;

vector<float> vertices;    //用于存储pcm文件解析出的数据
int istart=0;
int wstart=0;
int pstart=0;
int n;       //记录pcm文件中数据个数

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void drawLine();
void drawPoint();
void drawWave();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//PCM文件数据解码保存到数组中
void fileOutput()
{
    short pcm_In = 0;
    int size = 0;
    FILE *fp = fopen(OLD_FILE_PATH, "rb+");     //为读写打开一个二进制文件 即pcm文件
    
    int i=0;
    while(!feof(fp))
    {
        size = fread(&pcm_In, 2, 1, fp);     //pcm中每个数据大小为2字节，每次读取1个数据
        if(size>0)
        {
            //-------------------------------------------------------------------------------------------------------------------------
            if(pcm_In<0){
                pcm_In=-pcm_In;
            }
            vertices.push_back((float)pcm_In/30000);
        }
        i++;
    }
    
    n=i;
    //    cout<<"数据个数： "<<n<<endl;
    
    fclose(fp);
}

int main()
{
    fileOutput();
    
    // glfw: 初始化并配置
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // ！！！for OS X   没有此行报错为 not support version 330
#endif
    
    // glfw 创建窗口
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Music Circular Spectrum", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad: 家在所有OpenGL函数指针
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    
    // 构建并编译着色器程序
    // ------------------------------------
    Shader ourShader("/Users/boone/Desktop/Github/OpenGL/CODE/Spectrum/Spectrum2.0/spectrum.vs", "/Users/boone/Desktop/Github/OpenGL/CODE/Spectrum/Spectrum2.0/spectrum.fs");
    
    float* arr = new float[6*n];
    float* arr1 = new float[6*n];
    float* arr2 = new float[3*n];

    
    int i=0;
    float xstart=-1.0;
    
    //直线型频谱图数据存储
    for(vector<float>::iterator it = vertices.begin(); it != vertices.end(); it+=2 )    //用迭代器的方式输出容器对象的值
    {
        arr[i++]=xstart;     //圆上的点
        arr[i++]=0.0f;
        arr[i++]=0.0f;
        
        arr[i++]=xstart;     //由圆向外延伸的终点，表示频谱
        arr[i++]=*it;
        arr[i++]=0.0f;
        
        xstart=xstart+0.002;
        if (xstart>1.0) {
            xstart=-1.0;     //循环存储N个圆形频谱
        }
        
    }
    
    //离散点频谱图数据存储
    xstart=-1.0;
    i=0;
    for(vector<float>::iterator it = vertices.begin(); it != vertices.end(); it+=2 )    //用迭代器的方式输出容器对象的值
    {
        arr1[i++]=xstart;
        arr1[i++]=-*it-0.01;
        arr1[i++]=0.0f;
        
        arr1[i++]=xstart;
        arr1[i++]=*it+0.01;
        arr1[i++]=0.0f;
        
        xstart=xstart+0.005;
        if (xstart>1.0) {
            xstart=-1.0;
        }
    }

    //波形频谱图数据存储
    xstart=-1.0;
    i=0;
    for(vector<float>::iterator it = vertices.begin(); it != vertices.end(); it+=2 )    //用迭代器的方式输出容器对象的值
    {
        arr2[i++]=xstart;
        arr2[i++]=-*it;
        arr2[i++]=0.0f;
        
        xstart=xstart+0.001;
        if (xstart>1.0) {
            xstart=-1.0;
        }
        
    }

    //直线型
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, 24*n, arr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    
    //离散型
    unsigned int pointVBO, pointVAO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    
    glBufferData(GL_ARRAY_BUFFER, 24*n, arr1, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(0);

    
    //波形
    unsigned int waveVAO,waveVBO;
    glGenVertexArrays(1, &waveVAO);
    glGenBuffers(1, &waveVBO);
    
    glBindVertexArray(waveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waveVBO);
    
    glBufferData(GL_ARRAY_BUFFER, 12*n, arr2, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(0);

    
    // 循环渲染
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        // 渲染
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ourShader.use();
        
        glBindVertexArray(VAO); // 激活VAO表示的顶点缓存
        if (istart<6*n) {   //到达终点之前每次绘制一帧的频谱图
            drawLine();
        }

        glBindVertexArray(pointVAO); // 激活VAO表示的顶点缓存
        if (pstart<6*n) {   //到达终点之前每次绘制一帧的频谱图
            drawPoint();
        }
        
        glBindVertexArray(waveVAO); // 激活VAO表示的顶点缓存
        if (wstart<3*n) {   //到达终点之前每次绘制一帧的频谱图
            drawWave();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glDeleteVertexArrays(1, &waveVAO);
    glDeleteBuffers(1, &waveVBO);
    
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: 每当窗口大小改变时，调用该回调函数
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
//绘制频谱
void drawLine()
{
    usleep(99900);   //通过延时实现频谱的显示频率
    
    //颜色随机设置
    float redValue = 0.0f;
    float blueValue = 1.0f;
    
    for (int i=istart; i<2000+istart; i+=2) {
        
        glUniform4f(0, redValue, 1.0f, blueValue, 1.0f);
        
        if (i<=1000+istart) {
            redValue=redValue+0.002;
            blueValue=blueValue-0.002;
        }else{
            redValue=redValue-0.002;
            blueValue=blueValue+0.002;
        }
        
        glDrawArrays(GL_LINES, i, 2);
    }
    
    istart+=2000;
}

//绘制离散型频谱
void drawPoint()
{
    
    for (int i=pstart; i<800+pstart; i++) {
        
        glUniform4f(0, 0.2f, 0.7f, 1.0f, 1.0f);
        
        glPointSize(3);
        glDrawArrays(GL_POINTS, i, 1);
    }
    
    pstart+=800;
}
//绘制波形频谱
void drawWave()
{
    //颜色随机设置
    float redValue = 0.0f;
    float blueValue = 1.0f;
    
    for (int i=wstart; i<2000+wstart; i++) {
        
        glUniform4f(0, redValue, blueValue, 1.0f, 1.0f);
        
        if (i<=500+wstart) {
            redValue=redValue+0.002;
            blueValue=blueValue-0.002;
        }else{
            redValue=redValue-0.002;
            blueValue=blueValue+0.002;
        }

        //glLineWidth(8);
        glPointSize(4);
        glDrawArrays(GL_POINTS, i, 1);
    }
    
    wstart+=2000;
}
