#include <QtTest>

class PlaceholderTest : public QObject {
    Q_OBJECT

private slots:
    void sanityCheck() {
        QVERIFY(1 + 1 == 2);
    }
};

QTEST_MAIN(PlaceholderTest)
#include "tst_placeholder.moc"