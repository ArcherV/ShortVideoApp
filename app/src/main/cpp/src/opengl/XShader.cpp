//
// Created by ArcherV on 2021-02-22.
//

#include "XShader.h"
#include "../utils/Xlog.h"
#include <GLES2/gl2.h>

static GLuint InitShader(const char *code, GLuint type)
{
    //创建shader
    GLuint sh = glCreateShader(type);
    if(sh == 0){
        XLOGE("Init", "glCreateShader %d failed!", type);
        return 0;
    }

    //加载shader
    glShaderSource(sh, 1, &code, 0);
    //编译shader
    glCompileShader(sh);

    //获取编译情况
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if(status == 0){
        XLOGE("Init", "glCompileShader failed!");
        return 0;
    }
    XLOGE("Init", "glCompileShader success!");
    return sh;
}

void XShader::Close()
{
    std::lock_guard<std::mutex> lck(mux);
    //释放shader
    if(program)
        glDeleteProgram(program);

    if(fsh)
        glDeleteShader(fsh);

    if(vsh)
        glDeleteShader(vsh);
}

bool XShader::Init(const char *vertexShader, const char *fragmentShader)
{
    Close();
    std::lock_guard<std::mutex> lck(mux);

    //顶点和片元shader初始化
    //顶点shader初始化
    vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    if(vsh == 0){
        XLOGE("Init", "InitShader GL_VERTEX_SHADER failed!");
        return false;
    }

    XLOGE("Init", "InitShader GL_VERTEX_SHADER success!");

    fsh = InitShader(fragmentShader, GL_FRAGMENT_SHADER);

    if(fsh == 0){
        XLOGE("Init", "InitShader GL_FRAGMENT_SHADER failed!");
        return false;
    }
    XLOGE("Init", "InitShader GL_FRAGMENT_SHADER success!");


    //创建渲染程序
    program = glCreateProgram();
    if(program == 0){
        XLOGE("Init", "glCreateProgram failed！");
        return false;
    }

    //渲染程序中加入着色器代码
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    //链接程序
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE){
        XLOGE("Init", "glLinkProgram failed!");
        return false;
    }
    glUseProgram(program);
    XLOGE("Init", "glLinkProgram success!");


    //加入三维顶点数据 两个三角形组成正方形
    static float vers[] = {
            1.0f,   -1.0f,  0.0f,
            -1.0f,  -1.0f,  0.0f,
            1.0f,   1.0f,   0.0f,
            -1.0f,  1.0f,   0.0f,
    };

    GLuint  vpos = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vpos);
    //传递顶点
    glVertexAttribPointer(vpos, 3, GL_FLOAT, GL_FALSE, 12, vers);

    //加入材质坐标数据
    static float txts[] = {
            1.0f, 0.0f,//右下
            0.0f,0.0f,
            1.0f,1.0f,
            0.0,1.0
    };
    GLuint tex = (GLuint)glGetAttribLocation(program,"inputTextureCoordinate");
    glEnableVertexAttribArray(tex);
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 8, txts);

    //材质纹理初始化
    //设置纹理层
    glUniform1i(glGetUniformLocation(program, "s_texture"), 0); //对于纹理第1层

    XLOGE("Init", "初始化Shader成功!");
    return true;
}

void XShader::Draw(TextureType type)
{
    std::lock_guard<std::mutex> lck(mux);
    if(!program) {
        return;
    }
    //三维绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0 ,4);
}

void XShader::GetTexture(int textureId, int width, int height, unsigned char *buf)
{
    std::lock_guard<std::mutex> lck(mux);

    //激活第一层纹理 绑定到创建的opengl纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    //替换纹理内容
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
}