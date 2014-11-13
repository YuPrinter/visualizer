#ifndef VOLUMEMODEL_H
#define VOLUMEMODEL_H

#include "Info/PointsInfo.h"
#include "Info/VolumeInfo.h"

#include "Model/AbstractModelWithPoints.h"
#include "Model/VertexVT.h"

namespace Model {
    class VolumeModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit VolumeModel(Scene::AbstractScene * scene,
                             PointsModel * points = nullptr,
                             AbstractModel * parent = nullptr,

                           const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                    ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/Volume/vertex.glsl"),
                    ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/Volume/fragment.glsl")
                    << ShaderInfo::FragmentShaderFile(":shaders/Helpers/fragment.glsl")
                    ),

                           const ShaderInfo::ShaderVariablesNames & attributeArrays =
                ShaderInfo::ShaderVariablesNames() << "vertex" << "tex",

                           const ShaderInfo::ShaderVariablesNames & uniformValues =
                ShaderInfo::ShaderVariablesNames() << "view" << "model" << "projection" <<
                "scale" << "eye" << "modelBillboard" << "lightView");

        void init(const TextureInfo::Size & size, const VolumeInfo::PhysicalSize & physicalSize, const TextureInfo::Scaling & scaling);

        virtual Camera::Matrix mvp(const Viewport::Viewport * viewport) const;
        virtual Camera::Matrix childsMVP(const Viewport::Viewport * viewport, const AbstractModel * child) const;

        virtual Camera::ViewMatrix lightView(const Viewport::Viewport * viewport) const;

        virtual Camera::ModelMatrix model(const Viewport::Viewport * viewport) const;

    protected:
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;
        
        virtual void drawingRoutine() const;
    };
}
#endif // VOLUMEMODEL_H
