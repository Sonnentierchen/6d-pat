#include "objectmodelrenderable.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

ObjectModelRenderable::ObjectModelRenderable(const ObjectModel &objectModel,
                                             int vertexAttributeLoc,
                                             int normalAttributeLoc) :
    objectModel(objectModel),
    vertexAttributeLoc(vertexAttributeLoc),
    normalAttributeLoc(normalAttributeLoc) {

    rotation.setToIdentity();
    position = QVector3D(0, 0, 0);
    computeModelMatrix();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objectModel.getAbsolutePath().toStdString(),
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType
                                             );
    for (uint i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[0]);
    }
    populateVertexArrayObject();
}

QOpenGLVertexArrayObject *ObjectModelRenderable::getVertexArrayObject() {
    return &vao;
}

QMatrix4x4 ObjectModelRenderable::getModelMatrix() {
    return modelMatrix;
}

int ObjectModelRenderable::getIndicesCount() {
    return indices.size();
}

ObjectModel ObjectModelRenderable::getObjectModel() {
    return objectModel;
}

QVector3D ObjectModelRenderable::getPosition() {
    return  position;
}

void ObjectModelRenderable::setPosition(QVector3D position) {
    this->position = position;
    computeModelMatrix();
}

QMatrix3x3 ObjectModelRenderable::getRotation() {
    return rotation;
}

void ObjectModelRenderable::setRotation(QMatrix3x3 rotation) {
    this->rotation = rotation;
    computeModelMatrix();
}

// Private functions from here

void ObjectModelRenderable::computeModelMatrix() {
    float values[9] = {0.9981328845024109f,
                       -0.01055908389389515f,
                       0.06016005948185921f,
                       -0.004781095311045647f,
                       -0.9954285025596619f,
                       -0.09538961946964264f,
                       0.060892269015312195f,
                       0.09492388367652893f,
                       -0.9936204552650452f};
    QMatrix3x3 _rotation(values);
    modelMatrix = QMatrix4x4(_rotation);
    modelMatrix(0, 3) = -3.08480f;
    modelMatrix(1, 3) = 55.662601f;
    modelMatrix(2, 3) = 595.96228027f;
    QMatrix4x4 yz_flip;
    yz_flip.setToIdentity();
    yz_flip(1, 1) = -1;
    yz_flip(2, 2) = -1;
    modelMatrix = yz_flip * modelMatrix;
}

void ObjectModelRenderable::processMesh(aiMesh *mesh) {
    // Get Vertices
    if (mesh->mNumVertices > 0)
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mVertices[ii];

            vertices.push_back(vec.x);
            vertices.push_back(vec.y);
            vertices.push_back(vec.z);
        }
    }

    // Get Normals
    if (mesh->HasNormals())
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mNormals[ii];
            normals.push_back(vec.x);
            normals.push_back(vec.y);
            normals.push_back(vec.z);
        };
    }

    // Get mesh indexes
    for (uint t = 0; t < mesh->mNumFaces; ++t)
    {
        aiFace* face = &mesh->mFaces[t];
        if (face->mNumIndices != 3)
        {
            continue;
        }

        indices.push_back(face->mIndices[0]);
        indices.push_back(face->mIndices[1]);
        indices.push_back(face->mIndices[2]);
    }
}

void ObjectModelRenderable::populateVertexArrayObject() {
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Setup the vertex buffer object.
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexBuffer.allocate(vertices.constData(),
                                    vertices.size() * sizeof(GLfloat));
    vertexBuffer.bind();
    f->glEnableVertexAttribArray(vertexAttributeLoc);
    f->glVertexAttribPointer(vertexAttributeLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Setup the normal buffer object.
    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    normalBuffer.allocate(normals.constData(),
                                    normals.size() * sizeof(GLfloat));
    normalBuffer.bind();
    f->glEnableVertexAttribArray(normalAttributeLoc);
    f->glVertexAttribPointer(normalAttributeLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Setup the index buffer object.
    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer.allocate(indices.constData(),
                                   indices.size() * sizeof(GLint));
}

