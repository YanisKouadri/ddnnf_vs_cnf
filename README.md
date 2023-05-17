## Compilation approach on the stable semantic - D-DNNF vs CNF - Benchmarks

This project has been conducted for our distributed artificial intelligence master's first year's tutored project. We constructed a pipeline with the goal of confronting a d-dnnf approach by compilation and a direct cnf approach in the context of arguments acceptability in argumentation. This pipeline produces a csv comparing the performances of both approaches. Our complete workflow and results' analysis are available in the project's report (Not on github).   

⚠️ This project is for Linux Only because the executables used by the pipeline are Linux-compatible executables  
🛑 If you try to run on windows (do not), everything will work but the executables won't be launched, so you'll only have 1 empty transformation and query time and nothing else.  

⚠️ Since this project uses a lot of different subprocesses, it can be hard to manually interrupt. Ctrl-c will only end the current sub-process and will make the results biased. You can either spam it (can still be tricky sometimes) or simply close the terminal, which is the optimal solution.  

✍ There are a lot of comments in the code, do not hesitate to look into them or to contact us for any other issues.  

## ⚙️ Command :

cd in the project's root folder first, then :  
**python3 benchmark_st.py -task graphs nb_queries**    

task being dc for credulous acceptability or ds for skeptical acceptability.  
graphs being the name of the folder where the graphs to test are stored (put your instances in it, only .tgf will be considered)  
nb_queries being the number of queries per graph (randomly sampled arguments)  

example :   
python3 benchmark_st.py -dc graphs 10  

---

### 📦 Dependencies

Linux OS  
python3  

You don't need anything else since we only use executables and built-in modules.  
(optional) We left mu-toksia's full code beside the executable, if you want to modify it or generate a new build, you will need c++ on your system.  

### 📁 Folders' description

**Report_results** > contains the processed tables containing extra-columns that we presented in our report. Not needed at any point for the code, the generated results appear in the project's root folder and not here. The instances that we used are from : http://argumentationcompetition.org/ year 2021, folder 2019 (initially from 2019).  

**executables** > contains all our executables : the d4 compiler, the d-dnnf reasoner, and a folder containing all mu-toksia's code (do not get rid of it, needed for glucose or further modifications) and the executable's build.  

**generated** > a set of graphs we generated and used in our report. Not needed at any point for the code. You can try your tests on them if you are curious, but the ones named 0.3 will probably process for days (if not for ever since we timed them out in our tests). Format = GraphSize.Connectivity.OddCycles.EvenCycles.tgf  

**graphs** > the recommended folder to put your graphs in (you can provide any in the command line, this one is simply an option). Contains one graph as an example for users to test the script.  

**temp_files** > do not meddle with this folder, it is used several times in the pipeline by the executables to write and read the temporary files they need for the project to behave correctly. CNF_graph.cnf is used for each cnf encoding, DDNNF_graph.nnf is used for each d-dnnf representation, commands.txt is used to give instructions for all the queries in one go to the d-dnnf solver.  

**utils** > contains the different scripts of the project beside the main one. load_graph_cnf.py reads the graph and encode it to cnf, objects.py contains the graph and node objects, operations.py contains the methods for the d-dnnf transformation and queries.  

**benchmark_st.py** : main script  

### 🧰 Executables used

The d4 compiler : https://www.cril.univ-artois.fr/KC/d4.html  
The d-dnnf reasoner : http://www.cril.univ-artois.fr/kc/d-DNNF-reasoner.html  
µ-toksia (mu-toksia) : https://bitbucket.org/andreasniskanen/mu-toksia  

### 📝 Additional Precisions

- We use the graph format ".tgf" because the arguments' names are by default normalized and sorted. This way, when we want to add the relations of a given argument, we can directly access the corresponding object by index instead of searching for it (parsing the node list and comparing the names until its the one we're looking for), which would be computationally expensive. With this method, we don't need to handle any naming exception or preprocessing while reading.  

- In the last version of the d4 compiler, the output format for the d-dnnf representation has been modified (different from C2D), which means that its no longer compatible with the input format needed for the d-dnnf reasoner (our d-dnnf solver). Because of this fact, we are forced to use an old static version of d4. It will print wrong informations while parsing the graph, but the resulting models have been validated by comparing them to µ-toksia's, it is operational.  

- Detailed comments are provided to get a better understanding of the pipeline's steps. They are also explicited in the project's report (See Part 3).  

- The results csv will be generated in the project's root folder (same location as the benchmark_st.py script). The columns are the following : Graph Name, Total number of queries, was the Transformation UNSAT ?, d-dnnf transformation time, d-dnnf total queries time, then x columns (one for each mu-toksia cnf query), x being = to total number of queries in column 2. (The number of columns in the csv is initiated based on the user input, but the number of queries changes for each line, so they may be missing values depending on the line. total number of queries = user input or total number of args in the graph if it is smaller than user input) You can then process the csv manually as you wish to extract more informations, like the winning approach for each line and how many queries did it take to win (example : the two csv files in the Report_results folder).  
