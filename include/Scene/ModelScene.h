#ifndef MODELSCENE_H
#define MODELSCENE_H

#include "Scene/AbstractScene.h"

#include "Info/VolumeInfo.h"
#include "Info/TextureInfo.h"
#include "Info/PointsInfo.h"

#include "Model/StlModel.h"
#include "Model/VertexVN.h"

#include "Scene/BlueprintQueue.h"

#include "Viewport/ViewportArray.h"

namespace Scene {
    class ModelScene : public AbstractScene {
        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_PROPERTY(QVariant blueprint READ blueprint WRITE setBlueprint NOTIFY blueprintChanged)

        Q_OBJECT
    public:
        explicit ModelScene();
        explicit ModelScene(Viewport::ViewportArray ** viewPortArray);
        ~ModelScene();

        virtual void renderScene(const QSize & surfaceSize);

        virtual void cleanUp();

        Viewport::ViewportArray * viewportArray() const;        

        Model::AbstractModel * selectedModel() const;

        QVariant blueprint() const;

    protected:
        virtual void initScene();

    private:
        SceneObjectList<Model::AbstractModel *> _models;

        Viewport::ViewportArray * _viewportArray;

        QVariant _blueprint;

        BlueprintQueue _blueprints;

        void selectModel(Model::AbstractModel * model);

        void render(const Model::AbstractModel::RenderState & state = Model::AbstractModel::RenderState::CORE_RENDER);
        bool postProcess();

    signals:
        void viewportArrayChanged();

        void blueprintChanged(const QVariant & blueprint);

    public slots:
        virtual void setViewportArray(Viewport::ViewportArray * viewportArray);

        virtual Model::AbstractModel * addModel(const ModelInfo::Model & model);

        virtual void updateScene();

        virtual void recieve(const Message::SettingsMessage & message);

        virtual void setBlueprint(const QVariant & blueprint);
        virtual void unpackBlueprint(const Blueprint & blueprint, const bool & resetScene = false);
    };

}

#endif // MODELSCENE_H
