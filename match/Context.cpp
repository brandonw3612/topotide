#include "Context.h"
#include "boundaryreader.h"
#include "GraphComputer.h"

#include <iostream>
#include <QFileInfo>

#include "io/esrigridreader.h"
#include "io/gdalreader.h"
#include "io/textfilereader.h"

#include "NetworkConverter.h"
#include "NetworkGraphEdgeConnector.h"
#include "ReachNetworkDisplayFrame.h"

void Context::openFrames(QStringList& fileNames) {
    // Not reachable in CLI
    // if (m_computationRunning) {
    //     QMessageBox msgBox;
    //     msgBox.setIcon(QMessageBox::Critical);
    //     msgBox.setWindowTitle("Cannot open river");
    //     msgBox.setText("<qt>The river cannot be opened.");
    //     msgBox.setInformativeText("<qt>There is still a running computation. "
    //                               "Wait until the computation is finished, and "
    //                               "try again.");
    //     msgBox.exec();
    //     return;
    // }

    fileNames.sort();

    std::shared_ptr<RiverData> riverData;
    Units units;
    for (const QString& fileName : fileNames) {
        std::shared_ptr<RiverFrame> frame = loadFrame(fileName, units);
        if (!frame) {
            break;
        }
        if (riverData) {
            if (frame->m_heightMap.width() != riverData->width() ||
                frame->m_heightMap.height() != riverData->height()) {
                return;
                }
        } else {
            riverData =
                std::make_shared<RiverData>(frame->m_heightMap.width(), frame->m_heightMap.height(), units);
        }
        riverData->addFrame(frame);
    }

    if (!riverData || riverData->frameCount() == 0) {
        m_riverData = nullptr;
        // map->setRiverData(nullptr);
        // map->setRiverFrame(nullptr);
        // updateActions();
        return;
    }
    m_riverData = riverData;
    // m_frame = 0;
    // map->setRiverData(m_riverData);
    // map->setRiverFrame(activeFrame());
    // map->resetTransform();
    // backgroundDock->setElevationRange(m_riverData->minimumElevation(), m_riverData->maximumElevation());
    // unitsDock->setUnits(m_riverData->units());
    // progressDock->reset();
    // timeDock->setFrame(0);
    // timeDock->setFrameCount(m_riverData->frameCount());
    // updateActions();
    // markComputationNeeded(false);

    if (fileNames.length() == 1) {
        std::cout << "Opened river \"" << fileNames[0].toStdString() << "\"" << std::endl << std::endl;
    } else {
        std::cout << QString("Opened river containing %1 frames").arg(fileNames.length()).toStdString()
            << std::endl;
    }
}

std::shared_ptr<RiverFrame> Context::loadFrame(const QString& fileName, Units& units) {
    HeightMap heightMap;
    QString error = "[no error given]";
    if (fileName.endsWith(".txt")) {
        heightMap = TextFileReader::readTextFile(fileName, error, units);
    } else if (fileName.endsWith(".ascii") || fileName.endsWith(".asc")) {
        heightMap = EsriGridReader::readGridFile(fileName, error, units);
    } else {
        heightMap = GdalReader::readGdalFile(fileName, error, units);
    }

    if (heightMap.isEmpty()) {
        // something went wrong
        // QMessageBox msgBox;
        // msgBox.setIcon(QMessageBox::Critical);
        // msgBox.setWindowTitle("Cannot open river");
        // msgBox.setText(QString("<qt>The river <code>%1</code> cannot be opened.").arg(fileName));
        // msgBox.setInformativeText("<qt><p>This file does not seem to be an image file "
        //                         "or a valid text file containing elevation "
        //                         "data.</p>");
        // msgBox.setDetailedText("Reading the file failed due to the "
        //                     "following error:\n    " + error);
        // msgBox.exec();
        std::cerr << "Cannot open river: "
                  << QString("The river %1 cannot be opened.").arg(fileName).toStdString() << std::endl
                  << "This file does not seem to be an image file or a valid text file containing elevation data."
                  << std::endl << "Reading the text file failed due to the following error:" << std::endl
                  << "\t" << error.toStdString() << std::endl << std::endl;
        return nullptr;
    }

    return std::make_shared<RiverFrame>(fileName, heightMap);
}

void Context::openBoundary(const QString& fileName) {
    // QString fileName = QFileDialog::getOpenFileName(
    //             this,
    //             "Open river boundary",
    //             ".",
    //             "Boundary text files (*.txt)");

    if (fileName == nullptr) {
        return;
    }

    // if (m_computationRunning) {
    //     QMessageBox msgBox;
    //     msgBox.setIcon(QMessageBox::Critical);
    //     msgBox.setWindowTitle("Cannot open boundary");
    //     msgBox.setText("<qt>The river boundary cannot be opened.");
    //     msgBox.setInformativeText("<qt>There is still a running computation. "
    //                               "Wait until the computation is finished, and "
    //                               "try again.");
    //     msgBox.exec();
    //     return;
    // }

    QString error = "";
    Boundary boundary =
        BoundaryReader::readBoundary(fileName, m_riverData->width(), m_riverData->height(), error);

    if (error != "") {
        // something went wrong
        // QMessageBox msgBox;
        // msgBox.setIcon(QMessageBox::Critical);
        // msgBox.setWindowTitle("Cannot open boundary");
        // msgBox.setText(QString("<qt>The river boundary <code>%1</code> cannot be opened.").arg(fileName));
        // msgBox.setInformativeText("<qt><p>This file does not seem to be "
        //                           "a valid text file containing a river "
        //                           "boundary.</p>");
        // msgBox.setDetailedText("Reading the text file failed due to the "
        //                        "following error:\n    " + error);
        // msgBox.exec();
        std::cerr << "Cannot open boundary: "
                  << QString("The river boundary %1 cannot be opened.").arg(fileName).toStdString() << std::endl
                  << "This file does not seem to be a valid text file containing a river boundary." << std::endl
                  << "Reading the text file failed due to the following error:" << std::endl
                  << "\t" << error.toStdString() << std::endl << std::endl;
        return;
    }

    m_riverData->setBoundary(boundary);

    std::cout << "Opened boundary \"" << fileName.toStdString() << "\"" << std::endl;
    // map->update();
    // markComputationNeeded(false);
}

void Context::computeNetworkGraph() {
	if (!m_riverData->boundaryRasterized().isValid()) {
		// QMessageBox msgBox;
		// msgBox.setIcon(QMessageBox::Critical);
		// msgBox.setWindowTitle("Boundary invalid");
		// msgBox.setText("<qt>The computation cannot run as the boundary is invalid.");
		// msgBox.setInformativeText("<qt>A valid boundary does not self-intersect and does not visit "
		//                           "any points more than once. "
		//                           "Edit the boundary and try again.");
		// msgBox.exec();
		std::cerr << "Boundary invalid: The computation cannot run as the boundary is invalid." << std::endl
				  << "A valid boundary does not self-intersect and does not visit any points more than once. "
				  << "Edit the boundary and try again." << std::endl << std::endl;
		return;
	}
	// progressDock->reset();
	// m_computationRunning = true;
	// m_computationNeeded = false;

	// activeFrame()->m_inputGraph = nullptr;
	// activeFrame()->m_inputDcel = nullptr;
	// activeFrame()->m_msComplex = nullptr;
	// activeFrame()->m_networkGraph = nullptr;
	// map->update();
	// updateActions();

    for (int i = 0; i < m_riverData->frameCount(); i++) {
        auto frame = m_riverData->getFrame(i);
        GraphComputer gc(frame->m_name.toStdString(), m_riverData, frame);
        gc.start();
    }

    std::cout << std::endl;
}

void Context::buildAbstractionForAllFrames() {
    for (auto i = 0; i < m_riverData->frameCount(); i++) {
        auto frame = m_riverData->getFrame(i);
        QFileInfo fileInfo(frame->m_name);
        std::cout << "Converting " << fileInfo.baseName().toStdString() << std::endl;
        auto rn = NetworkConverter::ng2rn(frame->m_networkGraph);
        m_frames.push_back(std::make_shared<NetworkDisplayFrame>(fileInfo.baseName(), rn));
    }
}