# written by Tianji Liu for the CENG 4120 course project

import os
import argparse

class Cell:
    def __init__(self, locX, locY, width, height):
        assert isinstance(locX, float) or isinstance(locX, int)
        assert isinstance(locY, float) or isinstance(locY, int)
        assert isinstance(width, int) and isinstance(height, int)
        assert height in [8, 12]

        self.locX = locX
        self.locY = locY
        self.width = width
        self.height = height

class SolutionEvaluator:
    def __init__(self):
        self.clear()

    def clear(self):
        self.nSites = None
        self.nRows = None
        self.nTracks = None

        self.siteWidth = 0.216
        self.trackHeight = 0.27
        
        self.nCells = 0
        self.vCells = []
        self.name2CellId = {}
        
        self.vLegCells = []
        self.name2LegCellId = {}

        self.displBound = None


    def readInput(self, path):
        with open(path, "r") as f:
            data = f.read().splitlines()

        for i, line in enumerate(data):
            tokens = line.strip().split(" ")
            tokens = [ elem.strip() for elem in tokens ]

            if i == 0:
                self.nRows, self.nSites = int(tokens[0]), int(tokens[1])
            elif i == 1:
                self.trackHeight, self.siteWidth = float(tokens[0]), float(tokens[1])
            elif i == 2:
                self.displBound = float(tokens[0])
            elif i == 3:
                self.nCells = int(tokens[0])
            else:
                name = tokens[0]
                locY, locX = float(tokens[1]), float(tokens[2])
                h, w = int(tokens[3]), int(tokens[4])
                cell = Cell(locX, locY, w, h)
                self.name2CellId[name] = len(self.vCells)
                self.vCells.append(cell)
        assert len(self.vCells) == self.nCells
        assert len(self.vCells) == len(self.name2CellId)
        self.nTracks = 4 * self.nRows

    def readSolution(self, path):
        # this function needs the original cells (i.e., readInput()) read first
        with open(path, "r") as f:
            data = f.read().splitlines()

        nRowInfo = int(data[0].strip())
        data = data[nRowInfo + 1:]
        
        assert len(data) == self.nCells, \
            "#cells in the output is different from the input, or the row info has wrong format."

        for i, line in enumerate(data):
            tokens = line.strip().split(" ")
            tokens = [ elem.strip() for elem in tokens ]

            assert len(tokens) == 3, "Wrong line format."
            name = tokens[0]
            locY, locX = int(tokens[1]), int(tokens[2])
            assert locY % 4 == 0, "Y-location of a cell should be multiple of 4."

            assert name in self.name2CellId, "Mismatched cell name."
            cellId = self.name2CellId[name]
            origCell = self.vCells[cellId]

            legCell = Cell(locX, locY, origCell.width, origCell.height)
            self.name2LegCellId[name] = len(self.vLegCells)
            self.vLegCells.append(legCell)

        assert len(self.vLegCells) == len(self.vCells), \
            "#cells read from the output is different from the input."
        assert len(self.vLegCells) == len(self.name2LegCellId)

    def visualize(self, savePath, legal, displacement=False):
        """
        Args:
            legal (bool): whether to visualize the legalized solution
                or the initial placement.
            displacement(bool): whether to plot displacement vectors
                for each cell. Only valid when legal=True.
        """
        import matplotlib.pyplot as plt
        from matplotlib.patches import Rectangle
        
        assert (not displacement) or legal, "For showing displacement, legal must be set as True."

        if legal:
            vCells = self.vLegCells
        else:
            vCells = self.vCells

        fig, ax = plt.subplots()
        plt.xlim(0, self.nSites)
        plt.ylim(0, self.nTracks)

        if legal:
            # plot row lines
            vRowLocs = {cell.locY for cell in vCells}
            vRowLocs = sorted(list(vRowLocs))
            assert all((loc % 4 == 0) for loc in vRowLocs)
            ax.hlines(y=vRowLocs, xmin=0, xmax=self.nSites, color="lightgray", linewidth=0.5, zorder=0)
        else:
            # plot track lines
            ax.hlines(y=list(range(0, self.nTracks, 4)), xmin=0, xmax=self.nSites, 
                      color="lightgray", linewidth=0.5, zorder=0)

        # plot cells
        for cell in vCells:
            patch = Rectangle((cell.locX, cell.locY), cell.width, cell.height, 
                              facecolor="gray", edgecolor="blue", linewidth=0.3, zorder=1)
            ax.add_patch(patch)
        
        if displacement:
            assert len({self.nCells, len(self.vCells), len(self.vLegCells), 
                        len(self.name2CellId), len(self.name2LegCellId)}) == 1
            for cellLeg, cell in zip(self.vLegCells, self.vCells):
                plt.plot([cellLeg.locX + cellLeg.width / 2, cell.locX + cell.width / 2], 
                         [cellLeg.locY + cellLeg.height / 2, cell.locY + cell.height / 2],
                         linewidth=0.2, color="green", alpha=0.5)
        
        plt.savefig(savePath)

    def checkLegality(self):
        # this function should be called after the input and solution are both read
        print("Checking legality ...")
        vNames, vLowerLeft, vSizes, vUpperRight = [], [], [], []
        for name in self.name2LegCellId:
            cellId = self.name2LegCellId[name]
            cell = self.vLegCells[cellId]

            vNames.append(name)
            vLowerLeft.append([cell.locX, cell.locY])
            vSizes.append([cell.width, cell.height])
            vUpperRight.append([cell.locX + cell.width, cell.locY + cell.height])
        assert self.nCells == len(vNames)

        # sort the cells in y-coordinates and then x-coordinates
        vIndices = sorted(list(range(self.nCells)), key=lambda x : (vLowerLeft[x][1], vLowerLeft[x][0]))
        vNames = [vNames[i] for i in vIndices]
        vLowerLeft = [vLowerLeft[i] for i in vIndices]
        vSizes = [vSizes[i] for i in vIndices]
        vUpperRight = [vUpperRight[i] for i in vIndices]

        # check out-of-region cells
        print("Checking out-of-region cells ...")
        nOOB = 0
        for i in range(self.nCells):
            llx, lly = vLowerLeft[i]
            urx, ury = vUpperRight[i]
            if not (llx >= 0 and lly >= 0 and urx <= self.nSites and ury <= self.nTracks):
                nOOB += 1
        if nOOB > 0:
            print("Detected %d out-of-region cells." % nOOB)
            return False
        print("All the cells are in the placement region.")

        currTrack, currSite = 0, 0 # the upper-bound of the currently occupied track and site
        currRowLoc, currRowHeight = -1, -1
        for i in range(self.nCells):
            name = vNames[i]
            llx, lly = vLowerLeft[i]
            urx, ury = vUpperRight[i]
            w, h = vSizes[i]

            if lly != currRowLoc:
                # this cell starts a new row
                assert lly > currRowLoc
                # check whether the new row overlaps previous ones
                if lly < currTrack:
                    print("Overlapped row of cells when processing cell %s." % name)
                    return False
                # check whether the new row location is a multiple of 4
                if lly % 4 != 0:
                    print("The y-location of cell %s is not a multiple of 4." % name)
                    return False
                
                # no violation, start a new row of cells
                currRowLoc = lly
                currRowHeight = h
                currTrack = ury
                currSite = urx
            else:
                # this cell is still in the current row
                # check whether this cell has the same height of the current row
                if h != currRowHeight:
                    print("The height of cell %s is different from the height of this row." % name)
                    return False
                # check in-row overlap
                if llx < currSite:
                    print("In-row overlapping detected when processing cell %s." % name)
                    return False
                
                # no violation
                currSite = urx
        
        print("All constraints satisfied!")

        fSkipDebug = (self.nTracks * self.nSites > 10000 * 10000)
        try:
            import numpy as np
        except:
            fSkipDebug = True
        if fSkipDebug:
            return True

        assert self.debugCheckNonOverlap(vLowerLeft, vUpperRight), (
            "The debug checking found overlaps. This should not happen. "
            "If you see this, please send your input and solution case to TA. "
        )
        print("Debug checking passed!")
        return True

    def debugCheckNonOverlap(self, vLowerLeft, vUpperRight):
        import numpy as np
        print("Perform debug checking ...")

        assert len(vLowerLeft) == self.nCells
        assert len(vUpperRight) == self.nCells

        region = np.zeros((self.nTracks, self.nSites), dtype=np.int64)
        for i in range(self.nCells):
            llx, lly = vLowerLeft[i]
            urx, ury = vUpperRight[i]
            region[lly:ury, llx:urx] += 1
        
        return region.max() == 1
    
    def totalDisplacement(self):
        # this function should be called only when the legality checking has passed
        displacement = 0.0
        for name in self.name2LegCellId:
            cell = self.vCells[self.name2CellId[name]]
            cellLeg = self.vLegCells[self.name2LegCellId[name]]
            x1, y1 = cell.locX, cell.locY
            x2, y2 = cellLeg.locX, cellLeg.locY

            displacement += self.siteWidth * abs(x1 - x2) + self.trackHeight * abs(y1 - y2)
        return displacement

    def evalSolution(self, inPath, solPath):
        self.clear()
        try:
            self.readInput(inPath)
        except:
            print("The initial placement file is in wrong format. "
                  "This should not happen. Check your provided path.")
            return -2
        try:
            self.readSolution(solPath)
        except Exception as e:
            print("The solution file is in wrong format. The evaluation "
                  "cannot proceed unless a valid solution is provided.")
            print(">>> Error message when reading the solution:")
            print(">>> " + str(e))
            return -1

        print("Evaluating ...")
        
        # check whether the solution is legal
        if not self.checkLegality():
            print("Legality checking failed.")
            return 0
        
        score = 25
        # compute the total displacement
        displacement = self.totalDisplacement()
        print("Total displacement: %.4f, %s than the maximum total displacement (%.4f)." % 
              (displacement, "smaller" if displacement < self.displBound else "LARGER", self.displBound))
        if displacement < self.displBound:
            score = 50

        return score

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=str)
    parser.add_argument("solution", type=str)
    parser.add_argument("--plot", type=str, help="the directory to save the visualized result")
    parser.add_argument("--plot_displacement", action="store_true", 
                        help="if provided, also plot the displacement vector for each cell")
    args = parser.parse_args()

    evaluator = SolutionEvaluator()
    score = evaluator.evalSolution(args.input, args.solution)
    if score >= 0:
        print("score: %d" % score)

    if args.plot is not None:
        if score < 0:
            print("Failed in reading the input or solution file. Abort visualization.")
            exit(1)
        
        if os.path.exists(args.plot) and (not os.path.isdir(args.plot)):
            print("The provided path for visualization is not a directory. Abort visualization.")
            exit(1)
        
        if not os.path.exists(args.plot):
            os.makedirs(args.plot)
        evaluator.visualize(os.path.join(args.plot, "vis_input.pdf"), False)
        evaluator.visualize(os.path.join(args.plot, "vis_solution.pdf"), True, displacement=args.plot_displacement)
