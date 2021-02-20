//
// Created by admin on 2021-02-20.
//

#ifndef SHORTVIDEOAPP_XEGL_H
#define SHORTVIDEOAPP_XEGL_H


class XEGL {
public:
    virtual bool Init(void *win) = 0;
    virtual void Draw() = 0;
    virtual void Close() = 0;
    static XEGL *Get();

protected:
    XEGL(){}
};


#endif //SHORTVIDEOAPP_XEGL_H
