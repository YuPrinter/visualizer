#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtGui/QMatrix4x4>

#include <QtQuick/QQuickItem>

namespace Viewport {
    using ViewportRect = QRectF;

    class Viewport : public QQuickItem {
        Q_PROPERTY(QRectF boundingRect READ boundingRectNormalized
                   WRITE setBoundingRectNormalized NOTIFY boundingRectNormalizedChanged)

        Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
        Q_PROPERTY(QString text READ text CONSTANT)

        Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

        Q_ENUMS(ProjectionType)

        Q_OBJECT
    public:
        enum ProjectionType {
            PERSPECTIVE = 0,
            LEFT = 1,
            FRONTAL = 2,
            TOP = 3
        };

        static bool unproject(const QVector3D & point, const QMatrix4x4 & mvp, QVector4D & unprojectedPoint);

        explicit Viewport();
        explicit Viewport(const ViewportRect & boundingRectNormalized,
                          const QSize & surfaceSize,
                          const ProjectionType & projectionType = LEFT);

        ViewportRect boundingRect() const;

        ViewportRect boundingRectNormalized() const;

        ProjectionType projectionType() const;

        QMatrix4x4 modelBillboard(const QMatrix4x4 & model) const;

        QMatrix4x4 view() const;
        QMatrix4x4 viewBillboard() const;

        QVector3D eye() const;

        QMatrix4x4 projection() const;

        QString text() const;

        qreal zoom() const;

    private:
        QSize _surfaceSize;

        QRectF _boundingRectNormalized;

        ProjectionType _projectionType;

        QMatrix4x4 _vMatrix;
        QMatrix4x4 _vMatrixBillboard;

        QMatrix4x4 _pMatrix;

        QQuaternion _orientationBillboard;

        QVector3D _orientation;
        QVector3D _eye;
        QVector3D _center;
        QVector3D _up;

        qreal _fov;
        qreal _fovZoom;

        qreal _side;

        qreal _zoomFactor;

        qreal _aspectRatio;

        qreal _nearVal;
        qreal _farVal;

        QString _text;

        QVector4D calculateRayDir(const QVector4D & point) const;

        void ortho(const float & left, const float & right, const float & bottom,
                   const float & top, const float & nearVal, const float & farVal);

        void perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal);

    signals:
        void boundingRectNormalizedChanged();
        void projectionTypeChanged();

        void zoomChanged();

    public slots:
        virtual void setBoundingRectNormalized(const QRectF & boundingRectNormalized) final;

        virtual void setBoundingRect(const QRect & boundingRect) final;

        virtual void setProjectionType(const ProjectionType & projectionType) final;

        virtual void lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up) final;

        virtual void setZoom(const qreal & zoomFactor) final;

        virtual void resize(const QSize & windowSize) final;

        virtual void setProjection() final;
    };
}
#endif // VIEWPORT_H
