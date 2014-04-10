#include <QtGui/QColor>
#include <QtGui/QScreen>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "glviewer.h"

void gpu_profiling(const GPU_Driver & gpu_driver, const QString & debugMessage) {
   GLint nCurAvailMemoryInKB = 0;
   GLint nTotalMemoryInKB = 0;

   switch (gpu_driver) {
   case NVidia_binary :
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

       glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, & nCurAvailMemoryInKB);
       glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, & nTotalMemoryInKB);

#undef GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX
#undef GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX
       break;
   case NVidia_opensourse :
   case AMD_opensource :
   case AMD_binary :
   case Intel_opensource :
       break;
   }

   qDebug() << debugMessage
            << "Total: " << nTotalMemoryInKB / 1024
            << "MiB, Used:" << (nTotalMemoryInKB - nCurAvailMemoryInKB) / 1024
            << "MiB, Free:" << nCurAvailMemoryInKB / 1024 << "MiB";
}

GLviewer::GLviewer(const QSurfaceFormat & surfaceFormat, QWindow * parent) :
    OpenGLWindow(surfaceFormat, parent),
    _program(0),
    _textureCV3D(QOpenGLTexture::Target3D),
    _rBottom((GLfloat) 0.1),
    _rTop((GLfloat) 0.8),
    _ambientIntensity((GLfloat) 12.0),
    _lightPos(QVector3D(0.0, 0.0, -10.0)),
    _gpu_driver(NVidia_binary) {

    fetchHud();
}

void GLviewer::drawSlices(const uchar * mergedData, const std::vector<float> & scaling, const std::vector<size_t> &size,
                          const int & alignment, const size_t & rowLength) {
    _mergedData = mergedData;
    _scaling = scaling;
    _size = size;

    _alignment = alignment;
    _rowLength = rowLength;

    _program = 0;

    _matrixStack.identity();
    _matrixStack.ortho(-1.0, 1.0, -1.0, 1.0, 0.001, 1000.0);
    _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));

    _matrixStack.scale(QVector3D(_scaling[0], _scaling[1], _scaling[2]));

    _hud->show();
    show();
}

GLviewer::~GLviewer() {
    _textureCV3D.release();
}

void GLviewer::keyPressEvent(QKeyEvent * event) {
    int key = event->key();

    switch (key) {
        case Qt::Key_H:
            if (_hud->isVisible()) {
                _hud->hide();
            }
            else {
                _hud->show();
            }
    }

    event->accept();
}

void GLviewer::fetchHud() {
    _hud = new Hud(this);

    QObject::connect(_hud, &Hud::rBottomChanged, this, &GLviewer::updateRBottom);
    QObject::connect(_hud, &Hud::rTopChanged, this, &GLviewer::updateRTop);

    QObject::connect(_hud, &Hud::angleChanged, this, &GLviewer::updateAngle);
    QObject::connect(_hud, &Hud::zoomZChanged, this, &GLviewer::updateZoomZ);
    QObject::connect(_hud, &Hud::ambientIntensityChanged, this, &GLviewer::updateAmbientIntensity);
}

void GLviewer::initialize() {
    gpu_profiling(_gpu_driver, "initialization begin");

    _program = new QOpenGLShaderProgram(this);
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/vertex.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/fragment.glsl");
    _program->link();

    _shaderModel = _program->uniformLocation("model");
    _shaderView = _program->uniformLocation("view");
    _shaderProjection = _program->uniformLocation("projection");
    _shaderScale = _program->uniformLocation("scale");
    _shaderNormalMatrix = _program->uniformLocation("normalMatrix");
    _shaderLightPos = _program->uniformLocation("lightPos");
    _shaderAmbientIntensity = _program->uniformLocation("ambientIntensity");
    _shaderTexSample = _program->uniformLocation("texSample");
    _shaderRBottom = _program->uniformLocation("rBottom");
    _shaderRTop = _program->uniformLocation("rTop");

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initTextures();

    _glHeadModel.init(_program, _size[2]);

    gpu_profiling(_gpu_driver, "initialization end");
}

void GLviewer::render() {
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    _hud->resize(width() * retinaScale, 0.2 * height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    _program->bind();

    _program->setUniformValue(_shaderModel, _matrixStack.model());
    _program->setUniformValue(_shaderView, _matrixStack.view());
    _program->setUniformValue(_shaderProjection, _matrixStack.projection());
    _program->setUniformValue(_shaderScale, _matrixStack.scaleM());
    _program->setUniformValue(_shaderNormalMatrix, _matrixStack.normalM());
    _program->setUniformValue(_shaderLightPos, _lightPos);
    _program->setUniformValue(_shaderAmbientIntensity, _ambientIntensity);
    _program->setUniformValue(_shaderTexSample, 0);
    _program->setUniformValue(_shaderRBottom, _rBottom);
    _program->setUniformValue(_shaderRTop, _rTop);

    _textureCV3D.bind();

    _glHeadModel.drawModel(_program);

    gpu_profiling(_gpu_driver, "actual drawing");

    _program->release();
}

void GLviewer::initTextures() {
    QOpenGLPixelTransferOptions pixelOptions;
    pixelOptions.setAlignment(_alignment);
    pixelOptions.setRowLength(_rowLength);

    _textureCV3D.create();
    _textureCV3D.setSize(_size[0], _size[1], _size[2]);
    _textureCV3D.setFormat(QOpenGLTexture::R8_UNorm);
    _textureCV3D.setMaximumLevelOfDetail(100);
    _textureCV3D.setMinimumLevelOfDetail(-100);

    _textureCV3D.allocateStorage();

    _textureCV3D.setData(QOpenGLTexture::Red, QOpenGLTexture::UInt8, (void *) _mergedData, &pixelOptions);

    _textureCV3D.setSwizzleMask(QOpenGLTexture::RedValue, QOpenGLTexture::RedValue, QOpenGLTexture::RedValue, QOpenGLTexture::RedValue);

    _textureCV3D.setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    _textureCV3D.setMagnificationFilter(QOpenGLTexture::Linear);
    _textureCV3D.setWrapMode(QOpenGLTexture::ClampToBorder);

    _textureCV3D.generateMipMaps();
}

void GLviewer::updateRBottom(qreal rBottom) {
    _rBottom = std::min(rBottom, (qreal) _rTop);
    _rTop = std::max(rBottom, (qreal) _rTop);
    renderNow();
}

void GLviewer::updateRTop(qreal rTop) {
    _rTop = std::max(rTop, (qreal) _rBottom);
    _rBottom = std::min(rTop, (qreal) _rBottom);
    renderNow();
}

void GLviewer::updateAngle(qreal xRot, qreal yRot, qreal zRot) {
    _matrixStack.rotate(QVector3D(xRot, yRot, zRot));
    renderNow();
}

void GLviewer::updateZoomZ(qreal dist) {
    _matrixStack.zoomZ(dist);
    renderNow();
}

void GLviewer::updateAmbientIntensity(qreal ambientIntensity) {
    _ambientIntensity = ambientIntensity;
    renderNow();
}
