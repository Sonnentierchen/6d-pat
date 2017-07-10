#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#include "objectimagecorrespondence.h"
#include "loadandstorestrategylistener.h"
#include "image.h"
#include "objectmodel.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

class LoadAndStoreStrategy {

protected:
    list<LoadAndStoreStrategyListener*> listeners;

public:

    virtual ~LoadAndStoreStrategy();

    /*!
     * \brief persistObjectImageCorrespondence Persists the given ObjectImageCorrespondence. The details of
     * how the data is persisted depends on the LoadAndStoreStrategy implementation.
     * \param objectImageCorrespondence the object image correspondence to persist
     * \param deleteCorrespondence indicates whether the correspondence should be persistently deleted
     * \return true if persisting the object image correspondence was successful, false if not
     */
    virtual bool persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) = 0;

    /*!
     * \brief loadImages Loads the images.
     * \return the list of images
     */
    virtual list<Image> loadImages() = 0;

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual list<ObjectModel> loadObjectModels() = 0;

    /*!
     * \brief loadCorrespondences Loads the correspondences at the given path. How the correspondences are stored depends on the
     * strategy.
     * \param images the images to insert as references into the respective correspondences
     * \param objectModels the object models to insert as reference into the respective correspondence
     * \return the list of all stored correspondences
     */
    virtual list<ObjectImageCorrespondence> loadCorrespondences(list<Image>* images, list<ObjectModel>* objectModels) = 0;

    /*!
     * \brief pathExists Checks whether the given path exists on the file system and is accessible.
     * \param path the path that is to be checked
     * \return true if the path exists and is accessible, false if not
     */
    bool pathExists(const string path);

    /*!
     * \brief addListener Adds a listener to this load and store strategy that will be notified if the underlying data changes somehow.
     * \param listener the listener to be added
     */
    void addListener(LoadAndStoreStrategyListener* listener);

    /*!
     * \brief removeListener Removes the listener from the listeners of this strategy.
     * \param listener the listener to be removed
     */
    void removeListener(LoadAndStoreStrategyListener* listener);

};

#endif // LOADANDSTORESTRATEGY_H
