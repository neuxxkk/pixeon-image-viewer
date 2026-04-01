#include <QtTest>
#include "controller/ImageProcessor.h"
#include "model/ImageModel.h"
#include "model/ImageCollection.h"

class PixeonTests : public QObject {
    Q_OBJECT

private slots:
    void testBrightnessContrast_data() {
        QTest::addColumn<int>("brightness");
        QTest::addColumn<int>("contrast");

        QTest::newRow("Neutral") << 0 << 0;
        QTest::newRow("Max Brightness") << 100 << 0;
        QTest::newRow("Min Brightness") << -100 << 0;
        QTest::newRow("Max Contrast") << 0 << 100;
        QTest::newRow("Min Contrast") << 0 << -100;
    }

    void testBrightnessContrast() {
        QFETCH(int, brightness);
        QFETCH(int, contrast);

        QImage img(100, 100, QImage::Format_RGB32);
        img.fill(Qt::gray);

        QImage result = ImageProcessor::applyBrightnessContrast(img, brightness, contrast);
        QCOMPARE(result.size(), img.size());
        QCOMPARE(result.format(), img.format());
        
        if (brightness > 0) {
            QVERIFY(qRed(result.pixel(50, 50)) >= qRed(img.pixel(50, 50)));
        } else if (brightness < 0) {
            QVERIFY(qRed(result.pixel(50, 50)) <= qRed(img.pixel(50, 50)));
        }
    }

    void testRotation() {
        QImage img(100, 200, QImage::Format_RGB32);
        img.fill(Qt::red);

        QImage rotated90 = ImageProcessor::rotate(img, 90);
        QCOMPARE(rotated90.width(), 200);
        QCOMPARE(rotated90.height(), 100);

        QImage rotated180 = ImageProcessor::rotate(img, 180);
        QCOMPARE(rotated180.width(), 100);
        QCOMPARE(rotated180.height(), 200);
    }

    void testImageModel() {
        ImageModel model;
        QCOMPARE(model.isLoaded(), false);
        QCOMPARE(model.brightness(), 0);
        QCOMPARE(model.contrast(), 0);
        QCOMPARE(model.rotation(), 0);

        model.setBrightness(50);
        QCOMPARE(model.brightness(), 50);
    }

    void testImageCollection() {
        ImageCollection collection;
        QCOMPARE(collection.hasImages(), false);
        QCOMPARE(collection.count(), 0);
    }
};

QTEST_MAIN(PixeonTests)
#include "tst_placeholder.moc"
