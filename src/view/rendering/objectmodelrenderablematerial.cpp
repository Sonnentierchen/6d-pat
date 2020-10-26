#include "objectmodelrenderablematerial.h"

#include <QVector3D>
#include <QVector4D>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QAbstractTextureImage>

ObjectModelRenderableMaterial::ObjectModelRenderableMaterial(Qt3DCore::QNode *parent, bool withTexture)
    : Qt3DRender::QMaterial(parent)
      , m_effect(new Qt3DRender::QEffect())
      , m_diffuseTexture(new Qt3DRender::QTexture2D())
      , m_ambientParameter(new Qt3DRender::QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
      , m_diffuseTextureParameter(new Qt3DRender::QParameter(QStringLiteral("diffuseTexture"), QVariant::fromValue(m_diffuseTexture)))
      , m_diffuseParameter(new Qt3DRender::QParameter(QStringLiteral("diffuse"), QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f)))
      , m_specularParameter(new Qt3DRender::QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.01f, 0.01f, 0.01f, 1.0f)))
      , m_shininessParameter(new Qt3DRender::QParameter(QStringLiteral("shininess"), 150.0f))
      , m_textureScaleParameter(new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f))
      , m_clicksParameter(new Qt3DRender::QParameter(QStringLiteral("clicks[0]"), QVariantList()))
      , m_clickColorsParameter(new Qt3DRender::QParameter(QStringLiteral("colors[0]"), QVariantList()))
      , m_clickCountParameter(new Qt3DRender::QParameter(QStringLiteral("clickCount"), 0))
      , m_useDiffuseTextureParameter(new Qt3DRender::QParameter(QStringLiteral("useDiffuseTexture"), QVariant::fromValue(withTexture)))
      , m_circumfenceParameter(new Qt3DRender::QParameter(QStringLiteral("circumfence"), 0.001f))
      , m_selectedParameter(new Qt3DRender::QParameter(QStringLiteral("selected"), QVector4D(0.f, 0.f, 0.f, 0.f)))
      , m_technique(new Qt3DRender::QTechnique())
      , m_renderPass(new Qt3DRender::QRenderPass())
      , m_shaderProgram(new Qt3DRender::QShaderProgram())
      , m_filterKey(new Qt3DRender::QFilterKey)
{
    m_shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/object.vert"))));
    m_shaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/object.frag"))));

    m_effect->addParameter(m_diffuseParameter);

    if (withTexture)
        m_effect->addParameter(m_diffuseTextureParameter);

    m_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_technique->graphicsApiFilter()->setMajorVersion(3);
    m_technique->graphicsApiFilter()->setMinorVersion(1);
    m_technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    m_filterKey->setParent(this);
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));

    m_technique->addFilterKey(m_filterKey);

    m_renderPass->setShaderProgram(m_shaderProgram);

    m_technique->addRenderPass(m_renderPass);

    m_effect->addTechnique(m_technique);

    m_effect->addParameter(m_ambientParameter);
    m_effect->addParameter(m_specularParameter);
    m_effect->addParameter(m_shininessParameter);
    m_effect->addParameter(m_textureScaleParameter);
    m_effect->addParameter(m_clickCountParameter);
    m_effect->addParameter(m_clicksParameter);
    m_effect->addParameter(m_clickColorsParameter);
    m_effect->addParameter(m_useDiffuseTextureParameter);
    m_effect->addParameter(m_circumfenceParameter);
    m_effect->addParameter(m_selectedParameter);

    setEffect(m_effect);
}

ObjectModelRenderableMaterial::~ObjectModelRenderableMaterial() {

}

QColor ObjectModelRenderableMaterial::ambient() const {
   return m_ambientParameter->value().value<QColor>();
}

QColor ObjectModelRenderableMaterial::specular() const {
    return m_specularParameter->value().value<QColor>();
}

float ObjectModelRenderableMaterial::shininess() const {
    return m_shininessParameter->value().toFloat();
}

Qt3DRender::QAbstractTexture *ObjectModelRenderableMaterial::diffuse() const {
    return m_diffuseParameter->value().value<Qt3DRender::QAbstractTexture *>();
}

float ObjectModelRenderableMaterial::textureScale() const {
    return m_textureScaleParameter->value().toFloat();
}

bool ObjectModelRenderableMaterial::isSelected() const {
    return m_selected;
}

void ObjectModelRenderableMaterial::setAmbient(const QColor &color) {
    m_ambientParameter->setValue(color);
    Q_EMIT ambientChanged(color);
}

void ObjectModelRenderableMaterial::setSpecular(const QColor &specular) {
    m_specularParameter->setValue(specular);
    Q_EMIT specularChanged(specular);
}

void ObjectModelRenderableMaterial::setShininess(float shininess) {
    m_shininessParameter->setValue(shininess);
    Q_EMIT shininessChanged(shininess);
}

void ObjectModelRenderableMaterial::setDiffuseTexture(Qt3DRender::QAbstractTexture *diffuse) {
    diffuse->setParent(this);
    m_diffuseTextureParameter->setValue(QVariant::fromValue(diffuse));
    Q_EMIT diffuseChanged(diffuse);
}

void ObjectModelRenderableMaterial::setTextureScale(float textureScale) {
    m_textureScaleParameter->setValue(textureScale);
    Q_EMIT textureScaleChanged(textureScale);
}

void ObjectModelRenderableMaterial::setDiffuseColor(const QColor &color) {
    m_diffuseParameter->setValue(color);
}

void ObjectModelRenderableMaterial::addClick(QVector3D click, QColor color) {
    m_clicks.append(click);
    m_clickColors.append(QVector3D(color.red() / 255.f, color.green() / 255.f, color.blue() / 255.f));
    m_clickCountParameter->setValue(m_clicks.count());
    QVariantList clicks;
    for (QVector3D click : m_clicks) {
        clicks << click;
    }
    m_clicksParameter->setValue(clicks);
    QVariantList colors;
    for (QVector3D color : m_clickColors) {
        colors << color;
    }
    m_clickColorsParameter->setValue(QVariantList() << colors);
}

void ObjectModelRenderableMaterial::setCirumfence(float circumfence) {
    m_circumfenceParameter->setValue(circumfence);
}

void ObjectModelRenderableMaterial::setSelected(bool selected) {
    m_selected = selected;
    if (selected) {
        m_selectedParameter->setValue(QVector4D(0.2, 0.1, 0.0, 0.0));
    } else {
        m_selectedParameter->setValue(QVector4D(0.0, 0.0, 0.0, 0.0));
    }
    Q_EMIT selectedChanged(selected);
}

void ObjectModelRenderableMaterial::removeClicks() {
    m_clicks.clear();
    m_clickColors.clear();
    m_clicksParameter->setValue(m_clicks.constData());
    m_clickColorsParameter->setValue(m_clickColors.constData());
    m_clickCountParameter->setValue(m_clicks.count());
}