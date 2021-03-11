//
// Created by ArcherV on 2021-02-22.
//

#include "XShader.h"
#include "Xlog.h"
#include "XUtils.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

const float vers[] = {
        1.0f,   -1.0f,  0.0f,
        -1.0f,  -1.0f,  0.0f,
        1.0f,   1.0f,   0.0f,
        -1.0f,  1.0f,   0.0f,
};
const float txts[] = {
        1.0f, 0.0f,//右下
        0.0f,0.0f,
        1.0f,1.0f,
        0.0,1.0
};

static GLuint InitShader(const char *code, GLuint type)
{
    //创建shader
    GLuint sh = glCreateShader(type);
    if(sh == 0){
        XLOGE("InitShader", "glCreateShader %d failed!", type);
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
        XLOGE("InitShader", "glCompileShader failed!");
        return 0;
    }
    XLOGE("InitShader", "glCompileShader success!");
    return sh;
}

void XShader::Close()
{
    std::lock_guard<std::mutex> lck(mux);
    //释放shader
    if(program2D)
        glDeleteProgram(program2D);
    if(programOES)
        glDeleteProgram(programOES);
}

bool XShader::Init(bool screen)
{
    Close();
    std::lock_guard<std::mutex> lck(mux);
    programOES = CreateProgram(TYPE_OES, screen, vertexShader, fragmentOESShader);
    program2D = CreateProgram(TYPE_2D, screen, vertexShader, fragmentShader);
    return !(programOES == 0 || program2D == 0);
}

GLuint XShader::CreateProgram(TextureType type, bool screen, const char *vertexShader, const char *fragmentShader) {
    //顶点和片元shader初始化
    //顶点shader初始化
    vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    if(vsh == 0){
        XLOGE("CreateProgram", "InitShader GL_VERTEX_SHADER failed!");
        return 0;
    }

    XLOGE("CreateProgram", "InitShader GL_VERTEX_SHADER success!");

    fsh = InitShader(fragmentShader, GL_FRAGMENT_SHADER);

    if(fsh == 0){
        XLOGE("Init", "InitShader GL_FRAGMENT_SHADER failed!");
        return 0;
    }
    XLOGE("CreateProgram", "InitShader GL_FRAGMENT_SHADER success!");


    //创建渲染程序
    program = glCreateProgram();
    if(program == 0){
        XLOGE("CreateProgram", "glCreateProgram failed！");
        return 0;
    }

    //渲染程序中加入着色器代码
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    glDeleteShader(vsh);
    glDeleteShader(fsh);
    //链接程序
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE){
        XLOGE("CreateProgram", "glLinkProgram failed!");
        return 0;
    }
    glUseProgram(program);
    XLOGE("CreateProgram", "glLinkProgram success!");

    GLuint  vpos = (GLuint)glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vpos);
    //传递顶点
    glVertexAttribPointer(vpos, 3, GL_FLOAT, GL_FALSE, 12, vers);

    //加入材质坐标数据
    GLuint tex = (GLuint)glGetAttribLocation(program,"inputTextureCoordinate");
    glEnableVertexAttribArray(tex);
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 8, txts);

    //材质纹理初始化
    //设置纹理层
    glUniform1i(glGetUniformLocation(program, "s_texture"), 0); //对于纹理第1层

    static float mMatrix[16];
    if (type == TYPE_OES) {
        if (screen) {
            matrixSetRotateM(mMatrix, 180, 0.0f, 1.0f, 0.0f);
            matrixRotateM(mMatrix, -90, 0.0f, 0.0f, 1.0f);
        }
        else {
            matrixSetRotateM(mMatrix, 180, 0.0f, 1.0f, 0.0f);
            matrixRotateM(mMatrix, 90, 0.0f, 0.0f, 1.0f);
        }
    } else if (type == TYPE_2D) {
//        matrixSetIdentityM(mMatrix);
        matrixSetRotateM(mMatrix, 180, 0.0f, 1.0f, 0.0f);
        matrixRotateM(mMatrix, 180, 0.0f, 0.0f, 1.0f);
    }
    glUniformMatrix4fv(glGetUniformLocation(program, "uMVPMatrix"), 1, GL_FALSE, mMatrix);

    XLOGE("CreateProgram", "初始化Shader成功!");

    return program;
}

void XShader::Draw(TextureType type)
{
    std::lock_guard<std::mutex> lck(mux);
    if (type == TYPE_OES) {
        if (programOES == 0) {
            XLOGE("Draw", "programOES = %d", programOES);
            XLOGE("Draw", "程序不对");
            return;
        }
        glUseProgram(programOES);
    } else if (type == TYPE_2D) {
        if (program2D == 0) {
            XLOGE("Draw", "程序不对");
            return;
        }
        glUseProgram(program2D);
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

void XShader::GetTexture(unsigned int texId) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texId);
}
