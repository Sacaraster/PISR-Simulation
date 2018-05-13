import subprocess
import os

def main():

    rootPath = '/home/chris/Research/PISR_Sim_NGpp/Sims/Comparisons/TSP_k-Subtours/grid_md2wrp/'

    for root,dirs,files in os.walk(rootPath):        
        for file in files:
            if file.startswith('runTrades'):

                cmd = root + '/' + './runTrades.sh'

                print cmd
                
                FNULL = open(os.devnull, 'w')
                subprocess.call(cmd, cwd=root, stdout=FNULL)

                print 'Analysis...'
                
                subprocess.call(['python', 'analysisScript.py'], cwd=root, stdout=FNULL)

                print '*****************************************************************'


if __name__ == '__main__':

    main()