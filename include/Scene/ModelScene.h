#ifndef MODELSCENE_H
#define MODELSCENE_H

#include "Scene/AbstractScene.h"

#include "Info/LightInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/VolumeInfo.h"
#include "Info/TextureInfo.h"
#include "Info/PointsInfo.h"

#include "Model/StlModel.h"
#include "Model/VertexVN.h"
#include "Model/ModelArray.h"

#include "Viewport/ViewportArray.h"

namespace Scene {
    class ModelScene : public AbstractScene {
        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_OBJECT
    public:
        explicit ModelScene();
        explicit ModelScene(Viewport::ViewportArray ** viewPortArray);
        ~ModelScene();

        virtual void renderScene(const QSize & surfaceSize);

        virtual void cleanUp();

        QVector3D rotation() const;

        Viewport::ViewportArray * viewportArray() const;        

        Model::AbstractModel * selectedModel() const;

    protected:
        virtual void initScene();

    private:
        QMap<QOpenGLTexture *, TextureInfo::Texture> _texturesInfo;

        Model::ModelArray _models;

        QMultiMap<Model::AbstractModel *, QOpenGLTexture *> _texturesInModel;
        QMultiMap<Model::AbstractModel *, MaterialInfo::Material *> _materialsInModel;

        QList<MaterialInfo::Material *> _materials;
        QList<LightInfo::LightSource *> _lightSources;
        QList<QOpenGLTexture *> _textures;

        Model::AbstractModel * _selectedModel;
        QOpenGLTexture * _selectedTexture;

        QVector3D _rotation;

        Viewport::ViewportArray * _viewportArray;

        void selectModel(Model::AbstractModel * model);

        void render(QListIterator<Model::AbstractModel *> & modelIterator,
                    QListIterator<Viewport::Viewport *> & viewportIterator);

        bool postProcess(QListIterator<Model::AbstractModel *> & modelIterator,
                         QListIterator<Viewport::Viewport *> & viewportIterator);

    signals:
        void modelIDChanged(const uint & modelID);

        void pointUpdated(const PointsInfo::UpdatedPoint & point);

        void viewportArrayChanged();

    public slots:
        virtual void setViewportArray(Viewport::ViewportArray * viewportArray);

        virtual void addModel(ModelInfo::BuffersVN buffers);
        virtual void addModel(VolumeInfo::Volume volume);

        virtual void addModel(const Model::ModelName & name, const Model::ModelParams & initParams);

        virtual void addTexture(TextureInfo::Texture & textureInfo);

        virtual void setRotation(const QVector3D & rotation);

        // to clip selected model
        virtual void setXRange(const ViewRangeInfo::ViewAxisRange & xRange);
        virtual void setYRange(const ViewRangeInfo::ViewAxisRange & yRange);
        virtual void setZRange(const ViewRangeInfo::ViewAxisRange & zRange);

        virtual void setHuRange(const VolumeInfo::HuRange & huRange);

        virtual void addPoint(const PointsInfo::Point & point);
        void togglePoint(const QString & point);

        virtual void updateScene();

        virtual void recievePackage(const Package::SettingsPackage & package);
    };

}

#endif // MODELSCENE_H
