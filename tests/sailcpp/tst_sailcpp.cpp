/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cstdlib>
#include <ctime>

#include <QObject>
#include <QtTest>

#include "geocpp/geocpp.h"
#include "sailcpp/panel.h"
#include "sailcpp/saildef.h"
#include "sailcpp/sailworker.h"

class tst_SailCpp : public QObject
{
    Q_OBJECT

private slots:
    void testSide();
    void testSpinnaker();
};

void tst_SailCpp::testSide()
{
    CSide side(0);

    // Empty.
    QCOMPARE(side.left(), 0);
    QCOMPARE(side.bottom(), 0);

    // 1 point.
    side.push_back(CPoint3d(1, 2, 3));
    QCOMPARE(side.left(), 1);
    QCOMPARE(side.bottom(), 2);

    // 2 points
    side.push_back(CPoint3d(2, 3, 4));
    QCOMPARE(side.left(), 1);
    QCOMPARE(side.bottom(), 2);

    // 2 points
    side.push_back(CPoint3d(-1, -2, -3));
    QCOMPARE(side.left(), -1);
    QCOMPARE(side.bottom(), -2);
}

void tst_SailCpp::testSpinnaker()
{
    // Create a spinnaker definition
    CSailDef spinDef;
    spinDef.sailType = SPINNAKER;
    spinDef.sailCut = HORIZONTAL;
    spinDef.luffL = 10000;  // 10m luff
    spinDef.footL = 8000;   // 8m foot
    spinDef.spinMaxWidth = 6000;  // 6m max width
    spinDef.spinShoulderHeight = 60;  // shoulder at 60% of luff
    spinDef.spinAsymmetry = 0;  // symmetric spinnaker
    spinDef.clothW = 900;  // 900mm cloth width
    spinDef.seamW = 13;    // 13mm seam width

    // Create the sail
    CSailWorker worker(spinDef);
    CPanelGroup sail = worker.makeSail();

    // Verify sail was created
    QVERIFY(sail.size() > 0);

    // Verify panels were generated (should have multiple panels for a 10m luff with 900mm cloth)
    QVERIFY(sail.size() >= 10);  // At least 10 panels for 10m / 0.9m cloth

    // Verify the sail has proper structure (each panel should have 4 sides)
    for (unsigned int i = 0; i < sail.size(); i++)
    {
        QVERIFY(sail[i].left.size() > 0);
        QVERIFY(sail[i].right.size() > 0);
        QVERIFY(sail[i].top.size() > 0);
        QVERIFY(sail[i].bottom.size() > 0);
    }

    // Test with asymmetric spinnaker
    spinDef.spinAsymmetry = 500;  // 500mm asymmetry
    CSailWorker asymWorker(spinDef);
    CPanelGroup asymSail = asymWorker.makeSail();
    QVERIFY(asymSail.size() > 0);
}

QTEST_MAIN(tst_SailCpp)
#include "tst_sailcpp.moc"
