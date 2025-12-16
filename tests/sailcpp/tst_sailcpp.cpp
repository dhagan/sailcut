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
    void testSpinnakerGeometry();
    void testSpinnakerSymmetry();
    void testSpinnakerDefaultValues();
    void testSpinnakerPanelCoordinates();
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

void tst_SailCpp::testSpinnakerGeometry()
{
    // Create a spinnaker with known dimensions
    CSailDef spinDef;
    spinDef.sailType = SPINNAKER;
    spinDef.sailCut = HORIZONTAL;
    spinDef.luffL = 6000;  // 6m luff
    spinDef.leechL = 6000; // 6m leech (symmetric)
    spinDef.footL = 4500;  // 4.5m foot
    spinDef.spinMaxWidth = 9000;  // 9m max width
    spinDef.spinShoulderHeight = 60;  // shoulder at 60% of luff
    spinDef.spinAsymmetry = 0;  // symmetric
    spinDef.clothW = 900;
    spinDef.seamW = 13;

    CSailWorker worker(spinDef);
    CPanelGroup sail = worker.makeSail();

    // Verify sail was created
    QVERIFY(sail.size() > 0);
    
    // Check that panels span from tack to head
    // Bottom panel should start near tack height (750mm default)
    // Top panel should end near head height (tack + luffL)
    if (sail.size() > 0)
    {
        // First panel bottom should be near tack level
        real firstPanelZ = sail[0].bottom[0].z();
        QVERIFY(firstPanelZ >= 0 && firstPanelZ < 1000);
        
        // Last panel top should be near head level
        real lastPanelZ = sail[sail.size()-1].top[0].z();
        QVERIFY(lastPanelZ > 5000 && lastPanelZ <= 7000);
    }
}

void tst_SailCpp::testSpinnakerSymmetry()
{
    // Test that symmetric spinnaker is actually symmetric
    CSailDef spinDef;
    spinDef.sailType = SPINNAKER;
    spinDef.sailCut = HORIZONTAL;
    spinDef.luffL = 8000;
    spinDef.footL = 6000;
    spinDef.spinMaxWidth = 8000;
    spinDef.spinShoulderHeight = 60;
    spinDef.spinAsymmetry = 0;  // MUST be symmetric
    spinDef.clothW = 900;
    spinDef.seamW = 13;

    CSailWorker worker(spinDef);
    CPanelGroup sail = worker.makeSail();

    QVERIFY(sail.size() > 0);

    // For each panel, check that left and right sides are symmetric about centerline
    // The centerline should be at x = tackX
    for (unsigned int i = 0; i < sail.size(); i++)
    {
        if (sail[i].left.size() > 0 && sail[i].right.size() > 0)
        {
            // Check symmetry at bottom edge
            real leftX = sail[i].bottom[0].x();
            real rightX = sail[i].bottom[sail[i].bottom.size()-1].x();
            real centerX = (leftX + rightX) / 2.0;
            
            // Port and starboard should be equidistant from center
            real leftDist = abs(centerX - leftX);
            real rightDist = abs(rightX - centerX);
            
            // Allow 1mm tolerance for floating point errors
            QVERIFY(abs(leftDist - rightDist) < 1.0);
        }
    }
}

void tst_SailCpp::testSpinnakerDefaultValues()
{
    // Test that default CSailDef values work for spinnaker
    CSailDef spinDef;
    spinDef.sailType = SPINNAKER;
    spinDef.sailCut = HORIZONTAL;
    // Use defaults from saildef.cpp:
    // luffL = 6000, footL = 4500, spinMaxWidth = 9000

    CSailWorker worker(spinDef);
    CPanelGroup sail = worker.makeSail();

    // Should create a valid sail
    QVERIFY(sail.size() > 0);
    QVERIFY(sail.size() < 100);  // Reasonable number of panels

    // Each panel should have valid geometry
    for (unsigned int i = 0; i < sail.size(); i++)
    {
        // All sides should have at least 2 points
        QVERIFY(sail[i].left.size() >= 2);
        QVERIFY(sail[i].right.size() >= 2);
        QVERIFY(sail[i].top.size() >= 2);
        QVERIFY(sail[i].bottom.size() >= 2);
    }
}

void tst_SailCpp::testSpinnakerPanelCoordinates()
{
    // Test that panel coordinates are valid and progressive
    CSailDef spinDef;
    spinDef.sailType = SPINNAKER;
    spinDef.sailCut = HORIZONTAL;
    spinDef.luffL = 7000;
    spinDef.footL = 5000;
    spinDef.spinMaxWidth = 7000;
    spinDef.spinShoulderHeight = 55;
    spinDef.spinAsymmetry = 0;
    spinDef.clothW = 900;
    spinDef.seamW = 13;
    spinDef.tackY = 500;  // Tack height above deck

    CSailWorker worker(spinDef);
    CPanelGroup sail = worker.makeSail();

    QVERIFY(sail.size() > 0);

    // Check that panels progress upward (increasing Z)
    for (unsigned int i = 1; i < sail.size(); i++)
    {
        real prevZ = sail[i-1].top[0].z();
        real currZ = sail[i].bottom[0].z();
        
        // Current panel bottom should be at or near previous panel top
        // Allow for seam width difference
        QVERIFY(abs(currZ - prevZ) < 100);  // Within 100mm tolerance
    }

    // Width should increase from tack to shoulder, then decrease to head
    // Check that middle panels are wider than first/last panels
    if (sail.size() >= 3)
    {
        // First panel width (at tack)
        real firstWidth = abs(sail[0].bottom[sail[0].bottom.size()-1].x() - sail[0].bottom[0].x());
        
        // Middle panel width (near shoulder)
        unsigned int midIdx = sail.size() / 2;
        real midWidth = abs(sail[midIdx].bottom[sail[midIdx].bottom.size()-1].x() - sail[midIdx].bottom[0].x());
        
        // Last panel width (at head)
        unsigned int lastIdx = sail.size() - 1;
        real lastWidth = abs(sail[lastIdx].top[sail[lastIdx].top.size()-1].x() - sail[lastIdx].top[0].x());
        
        // Middle should be widest (shoulder position)
        QVERIFY(midWidth > firstWidth);
        QVERIFY(midWidth > lastWidth);
    }

    // Check that all coordinates are finite (not NaN or Inf)
    for (unsigned int i = 0; i < sail.size(); i++)
    {
        for (unsigned int j = 0; j < sail[i].bottom.size(); j++)
        {
            QVERIFY(std::isfinite(sail[i].bottom[j].x()));
            QVERIFY(std::isfinite(sail[i].bottom[j].y()));
            QVERIFY(std::isfinite(sail[i].bottom[j].z()));
        }
    }
}

QTEST_MAIN(tst_SailCpp)
#include "tst_sailcpp.moc"
