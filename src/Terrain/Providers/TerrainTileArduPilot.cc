/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "TerrainTileArduPilot.h"
#include "QGCLoggingCategory.h"

#include <QtCore/QDataStream>
#include <QtPositioning/QGeoCoordinate>

QGC_LOGGING_CATEGORY(TerrainTileArduPilotLog, "qgc.terrain.terraintileardupilot");

TerrainTileArduPilot::TerrainTileArduPilot()
{
    // qCDebug(TerrainTileArduPilotLog) << Q_FUNC_INFO << this;
}

TerrainTileArduPilot::TerrainTileArduPilot(const QByteArray &byteArray)
    : TerrainTile(byteArray)
{
    // qCDebug(TerrainTileArduPilotLog) << Q_FUNC_INFO << this;
}

TerrainTileArduPilot::~TerrainTileArduPilot()
{
    // qCDebug(TerrainTileArduPilotLog) << Q_FUNC_INFO << this;
}

QList<QGeoCoordinate> TerrainTileArduPilot::parseCoordinateData(const QString &name, const QByteArray &hgtData)
{
    QString baseName = name.section('.', 0, 0);
    bool ok = false;

    // Parse Latitude
    if (baseName.size() < 4) {
        qCDebug(TerrainTileArduPilotLog) << "Invalid HGT file name format:" << name;
        return QList<QGeoCoordinate>();
    }

    QChar latHemisphere = baseName.at(0);
    QString latDegreesStr = baseName.mid(1, 2);
    int latDegrees = latDegreesStr.toInt(&ok);
    if (!ok) {
        qCDebug(TerrainTileArduPilotLog) << "Unable to parse latitude degrees from HGT file name:" << name;
        return QList<QGeoCoordinate>();
    }

    double baseLat = (latHemisphere == 'S') ? -latDegrees : latDegrees;

    // Parse Longitude
    QChar lonHemisphere = baseName.at(3);
    QString lonDegreesStr = baseName.mid(4, 3);
    int lonDegrees = lonDegreesStr.toInt(&ok);
    if (!ok) {
        qCDebug(TerrainTileArduPilotLog) << "Unable to parse longitude degrees from HGT file name:" << name;
        return QList<QGeoCoordinate>();
    }

    double baseLon = (lonHemisphere == 'W') ? -lonDegrees : lonDegrees;

    constexpr int kTileDimension = TerrainTileArduPilot::kTileDimension;
    constexpr int kTotalPoints = TerrainTileArduPilot::kTotalPoints;

    if (hgtData.size() != kTotalPoints * sizeof(qint16)) {
        qCDebug(TerrainTileArduPilotLog) << "Invalid HGT file size for file:" << name;
        return QList<QGeoCoordinate>();
    }

    QDataStream stream(hgtData);
    stream.setByteOrder(QDataStream::BigEndian);

    QList<QGeoCoordinate> coordinates;
    coordinates.reserve(kTotalPoints);

    for (int row = 0; row < kTileDimension; ++row) {
        for (int col = 0; col < kTileDimension; ++col) {
            qint16 elevationRaw;
            stream >> elevationRaw;

            if (stream.status() != QDataStream::Ok) {
                qCDebug(TerrainTileArduPilotLog) << "Error reading elevation data at row" << row << "col" << col << "in file:" << name;
                return QList<QGeoCoordinate>();
            }

            const double elevation = (elevationRaw == -32768) ? std::numeric_limits<double>::quiet_NaN() : static_cast<double>(elevationRaw);

            // Calculate current latitude (decreasing from north to south)
            const double currentLat = baseLat + 1.0 - (static_cast<double>(row) / (kTileDimension - 1));

            // Calculate current longitude (increasing from west to east)
            const double currentLon = baseLon + (static_cast<double>(col) / (kTileDimension - 1));

            if ((currentLat < -90.0) || (currentLat > 90.0) || (currentLon < -180.0) || (currentLon > 180.0)) {
                qCDebug(TerrainTileArduPilotLog) << "Calculated coordinate out of bounds:" << currentLat << currentLon << "in file:" << name;
                continue;
            }

            QGeoCoordinate coordinate(currentLat, currentLon, elevation);

            (void) coordinates.append(coordinate);
        }
    }

    if (stream.status() != QDataStream::Ok) {
        qCDebug(TerrainTileArduPilotLog) << "Stream ended prematurely while reading HGT file:" << name;
        return QList<QGeoCoordinate>();
    }

    return coordinates;
}

QByteArray TerrainTileArduPilot::serializeFromData(const QString &name, const QByteArray &hgtData)
{
    TerrainTile::TileInfo_t tileInfo;
    tileInfo.minElevation = std::numeric_limits<int16_t>::max();
    tileInfo.maxElevation = std::numeric_limits<int16_t>::min();
    tileInfo.avgElevation = 0.0;
    int validPoints = 0;

    const QString baseName = name.section('.', 0, 0);
    bool ok = false;

    if (baseName.length() < 7) {
        qCDebug(TerrainTileArduPilotLog) << "Invalid HGT file name format:" << name;
        return QByteArray();
    }

    const QChar latHemisphere = baseName.at(0).toUpper();
    const QString latDegreesStr = baseName.mid(1, 2);
    const int latDegrees = latDegreesStr.toInt(&ok);
    if (!ok || (latHemisphere != 'N' && latHemisphere != 'S')) {
        qCDebug(TerrainTileArduPilotLog) << "Unable to parse latitude from HGT file name:" << name;
        return QByteArray();
    }

    const double baseLat = (latHemisphere == 'S') ? -latDegrees : latDegrees;

    const QChar lonHemisphere = baseName.at(3).toUpper();
    const QString lonDegreesStr = baseName.mid(4, 3);
    const int lonDegrees = lonDegreesStr.toInt(&ok);
    if (!ok || (lonHemisphere != 'E' && lonHemisphere != 'W')) {
        qCDebug(TerrainTileArduPilotLog) << "Unable to parse longitude from HGT file name:" << name;
        return QByteArray();
    }

    const double baseLon = (lonHemisphere == 'W') ? -lonDegrees : lonDegrees;

    const int totalPoints = hgtData.size() / 2; // Each elevation point is 2 bytes (int16_t)
    const double gridSize = std::sqrt(static_cast<double>(totalPoints));

    // Check if gridSize is an integer
    if (std::floor(gridSize) != gridSize) {
        qCDebug(TerrainTileArduPilotLog) << "HGT file data size does not correspond to a square grid:" << name;
        return QByteArray();
    }

    tileInfo.gridSizeLat = static_cast<int16_t>(gridSize);
    tileInfo.gridSizeLon = static_cast<int16_t>(gridSize);

    tileInfo.swLat = baseLat;
    tileInfo.swLon = baseLon;
    tileInfo.neLat = baseLat + 1.0;
    tileInfo.neLon = baseLon + 1.0;

    QDataStream stream(hgtData);
    stream.setByteOrder(QDataStream::BigEndian);

    QList<int16_t> elevationGrid;
    elevationGrid.reserve(totalPoints);

    for (int i = 0; i < totalPoints; ++i) {
        qint16 elevationRaw;
        stream >> elevationRaw;

        if (stream.status() != QDataStream::Ok) {
            qCDebug(TerrainTileArduPilotLog) << "Error reading elevation data at point" << i << "in file:" << name;
            return QByteArray();
        }

        if (elevationRaw == -32768) {
            continue;
        }

        if (elevationRaw < tileInfo.minElevation) {
            tileInfo.minElevation = elevationRaw;
        }
        if (elevationRaw > tileInfo.maxElevation) {
            tileInfo.maxElevation = elevationRaw;
        }

        tileInfo.avgElevation += static_cast<double>(elevationRaw);
        validPoints++;

        elevationGrid.append(elevationRaw);
    }

    if (validPoints > 0) {
        tileInfo.avgElevation /= static_cast<double>(validPoints);
    } else {
        tileInfo.avgElevation = std::numeric_limits<double>::quiet_NaN();
    }

    if (validPoints == 0) {
        tileInfo.minElevation = 0;
        tileInfo.maxElevation = 0;
    }

    qCDebug(TerrainTileArduPilotLog) << "Serialize: TileInfo: SW Lat:" << tileInfo.swLat << "SW Lon:" << tileInfo.swLon;
    qCDebug(TerrainTileArduPilotLog) << "Serialize: TileInfo: NE Lat:" << tileInfo.neLat << "NE Lon:" << tileInfo.neLon;
    qCDebug(TerrainTileArduPilotLog) << "Min Elevation:" << tileInfo.minElevation << "meters";
    qCDebug(TerrainTileArduPilotLog) << "Max Elevation:" << tileInfo.maxElevation << "meters";
    qCDebug(TerrainTileArduPilotLog) << "Average Elevation:" << tileInfo.avgElevation << "meters";
    qCDebug(TerrainTileArduPilotLog) << "Grid Size (Lat x Lon):" << tileInfo.gridSizeLat << "x" << tileInfo.gridSizeLon;

    constexpr int cTileNumHeaderBytes = static_cast<int>(sizeof(TerrainTile::TileInfo_t));
    const int cTileNumDataBytes = static_cast<int>(sizeof(int16_t)) * tileInfo.gridSizeLat * tileInfo.gridSizeLon;

    QByteArray result(cTileNumHeaderBytes + cTileNumDataBytes, Qt::Uninitialized);

    TerrainTile::TileInfo_t *pTileInfo = reinterpret_cast<TerrainTile::TileInfo_t*>(result.data());

    *pTileInfo = tileInfo;

    int16_t* const pTileData = reinterpret_cast<int16_t*>(&reinterpret_cast<uint8_t*>(result.data())[cTileNumHeaderBytes]);

    for (int i = 0; i < elevationGrid.size(); ++i) {
        pTileData[i] = elevationGrid[i];
    }

    qCDebug(TerrainTileArduPilotLog) << "Serialization complete for tile:" << name;

    return result;
}
