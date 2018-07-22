#ifndef TEXTFILELOADANDSTORESTRATEGY_H
#define TEXTFILELOADANDSTORESTRATEGY_H

#include "loadandstorestrategy.hpp"
#include <QDir>
#include <QString>
#include <QStringList>
#include <QList>
#include <QFileSystemWatcher>

/*!
 * \brief The TextFileLoadAndStoreStrategy class is a simple implementation of a LoadAndStoreStrategy that makes no use of
 * caching already loaded images or object models and writes poses to text files. That is why it is important to set the
 * proper path to the folder of poses before using this strategy.
 */
class JsonLoadAndStoreStrategy : public LoadAndStoreStrategy
{

    Q_OBJECT

public:
    //! Unmodifiable constants (i.e. not changable by the user at runtime)
    static const QStringList IMAGE_FILES_EXTENSIONS;
    static const QStringList OBJECT_MODEL_FILES_EXTENSIONS;

public:
    /*!
     * \brief TextFileLoadAndStoreStrategy Constructor of this strategy. The paths MUST be set aferwards to use it
     * properly, otherwise the strategy won't deliver any content.
     */
    JsonLoadAndStoreStrategy();

    /*!
     * \brief TextFileLoadAndStoreStrategy Convenience constructor setting the paths already.
     * \param _imagesPath
     * \param _objectModelsPath
     * \param _posesPath
     */
    JsonLoadAndStoreStrategy(const QDir &imagesPath,
                             const QDir &segmentationImagesPath,
                             const QDir &objectModelsPath,
                             const QDir &posesFilePath);

    ~JsonLoadAndStoreStrategy();

    bool persistObjectImagePose(Pose *objectImagePose,
                                          bool deletePose) override;

    QList<Image> loadImages() override;

    QList<ObjectModel> loadObjectModels() override;

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses are stored depends on the
     * strategy.
     *
     * IMPORTANT: This implementation of LoadAndStoreStrategy makes use of text files to store poses, this means that the
     * path to the folder has to be set before this method is called. Failing to do so will raise an exception.
     *
     * \param images the images to insert as references into the respective poses
     * \param objectModels the object models to insert as reference into the respective pose
     * \param poses the list that the corresondences are to be added to
     * \return the list of all stored poses
     * \throws an exception if the path to the folder that should hold the poses has not been set previously
     */
    QList<Pose> loadPoses(const QList<Image> &images,
                                              const QList<ObjectModel> &objectModels) override;

    /*!
     * \brief setImagesPath Sets the path to the folder where the images that are to be annotated are located. After setting the
     * path the manager will automatically try to load the images and also their respective segmentation using the pattern for
     * segmentation image names.
     * \param path the path to the folder where the images are located
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setImagesPath(const QDir &path);

    /*!
     * \brief getImagesPath Returns the path that this manager uses to load images.
     * \return the path that this manager uses to load images
     */
    QDir getImagesPath() const;

    /*!
     * \brief setObjectModelsPath Sets the path to the folder where the object models are located. After setting the path the
     * manager will automatically try to load the objects.
     * \param path the path to the folder where the objects are located
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setObjectModelsPath(const QDir &path);

    /*!
     * \brief getObjectModelsPath Returns the path that this manager uses to load object models.
     * \return the path that this manager uses to load object models
     */
    QDir getObjectModelsPath() const;

    /*!
     * \brief setPosesPath Sets the path to the folder where object image poses are stored at. After setting
     * the path the manager will automatically try to load the already existing poses and will store all future
     * poses at this location.
     * \param path the path where the manager should store poses at or where some poses already exist that
     * are meant to be loaded
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setPosesFilePath(const QDir &path);

    /*!
     * \brief getPosesPath Returns the path that this manager uses to store and load poses.
     * \return the path that this manager uses to store and load poses
     */
    QDir getPosesFilePath() const;

    /*!
     * \brief setSegmentationImageFilesSuffix sets the given suffix as the suffix to be used
     * when loading segmentation images. Segmentation images have to be in the same folder as
     * the actual images.
     * \param suffix the suffix to be used for segmentation images
     */
    void setSegmentationImagesPath(const QDir &path);

    /*!
     * \brief getSegmentationImageFilesSuffix returns the suffix that is used to load
     * segementation images from the folder of images.
     * \return the suffix that is used to load segmentation images
     */
    QDir getSegmentationImagesPath();

private Q_SLOTS:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &filePath);

private:

    //! Stores the path to the folder that holds the images
    QDir imagesPath;
    //! Stores the path to the folder that holds the object models
    QDir objectModelsPath;
    //! Stores the path to the already created poses
    QDir posesFilePath;
    //! Stores the suffix that is used to try to load segmentation images
    QDir segmentationImagesPath;

    QFileSystemWatcher watcher;

    void connectWatcherSignals();
};

#endif // TEXTFILELOADANDSTORESTRATEGY_H
