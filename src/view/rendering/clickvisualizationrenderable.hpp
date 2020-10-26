#ifndef CLICKVISUALIZATIONRENDERABLE_H
#define CLICKVISUALIZATIONRENDERABLE_H

#include "view/rendering/clickvisualizationmaterial.hpp"

#include <QSize>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>

class ClickVisualizationRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    ClickVisualizationRenderable(Qt3DCore::QNode *parent = Q_NULLPTR);

public Q_SLOTS:
    void addClick(QPoint click);
    void removeClick(QPoint click);
    void removeClicks();
    void setSize(QSize size);

private:
    Qt3DExtras::QPlaneMesh *mesh;
    Qt3DCore::QTransform *transform;
    ClickVisualizationMaterial *material;
};

#endif // CLICKVISUALIZATIONRENDERABLE_H
