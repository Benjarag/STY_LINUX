#!/usr/bin/env python3
import subprocess
import re
import matplotlib.pyplot as plt
#
# Put your solution into the three functions in this file
#

def get_page_list(filename):
    # Expected functionality: Read content of file (valgrind/lackey output), and then
    # - find all lines containing memory access:
    #   Line has I, L, M, S at the beginning (first two columns), then a space
    #   After that an address in hex notation
    #   Finally, a comma and an access size in byte
    # - construct an ordered list of memory pages accessed (based on the address)
    # - construct an set of memory pages that contain instructions (based on address in 'I' lines)
    page_access_list = []
    instruction_page_set = set()


    # Open the file containing valgrind/lackey output
    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            if not line:
                continue
            
            # Split the line into parts
            parts = line.split()
            # parts should be 2 e.g.: ['I', '04020204,7']
            if len(parts) != 2:
                continue # invalid
            
            if parts[0] not in "ILMS":
                continue # skipping the lines that do not start with types I, L, M, S

            # seperate the address and the size
            address, size = parts[1].split(',')
            address = int(address, 16) # convert the address from hex to int
            
            size = int(size) # convert the size from string to int

            # calculate the page number
            page_number = address >> 12

            # adding to the page_access_list
            page_access_list.append(page_number) 

            # if line is instruction then add the page number to the instruction_page_set
            if parts[0] == "I":
                instruction_page_set.add(page_number)
    
    return page_access_list, instruction_page_set


def export_page_trace(page_access_list, output_file):

    if not page_access_list:
        return # if no list, return
    # creating a new list
    new_page_acess_list = [page_access_list[0]]
    for i in range(1, len(page_access_list)):
        if page_access_list[i] != page_access_list[i-1]:
            new_page_acess_list.append(page_access_list[i])
    
    with open(output_file, 'w') as file:
        for page in new_page_acess_list:
            file.write(f"{page}\n")

    return new_page_acess_list

def simulate_page_replacement(page_access_list, memory_size, policy):
    '''
    Calls the paging-policy executable to simulate page replacement.
    Returns the number of page faults.
    '''
    page_list_str = ",".join(map(str, page_access_list)) # convert the list to a comma-separated string
    args = ['./paging-policy.py', '-a', page_list_str, '-C', str(memory_size), '-p', policy, '-c']
    try:
        result = subprocess.run(args, capture_output=True, text=True, check=True) # run the paging-policy executable
        output = result.stdout.strip() # get the output
        # search for the number of page faults in the output
        match = re.search(r'misses\s+(\d+)', output)
        if match:
            page_faults = int(match.group(1))
        else:
            page_faults = None
        return page_faults
    except subprocess.CalledProcessError as e:
        print(f"Error running paging-policy: {e}")
        return None

def run_simulations(page_access_list, policies=['FIFO', 'LRU', 'OPT']):
    '''
    For each physical memory size, and each replacement policy, run the simulation to get the number of page faults.
    Return a list of physical memory sizes tested, and a dictionary mapping each policy to a list of page fault counts for each physical memory size.
    '''
    unique_pages = set(page_access_list)
    num_unique_pages = len(unique_pages) 


    # list of physical memory sizes
    memory_sizes = list(range(1, num_unique_pages+1))
    # dictionary to store the results, with policy as key and list of page faults as value
    results = {policy: [] for policy in policies}

    for memory_size in memory_sizes:
        print(f"Running simulations for memory size {memory_size}")
        for policy in policies:
            faults = simulate_page_replacement(page_access_list, memory_size, policy)
            print(f"Policy {policy}, faults: {faults}")
            results[policy].append(faults)
    return memory_sizes, results


def plot_results(memory_sizes, results, png_file):
    """
    Plots a graph with:
       - X-axis: Number of pages in physical memory.
       - Y-axis: Number of page faults (log scale).
       
    Four curves (one for each policy) are plotted using different colors, and the graph is saved to png_file.
    """
    plt.figure(figsize=(10, 6))
    colors = {'FIFO': 'blue', 'LRU': 'green', 'RAND': 'red', 'OPT': 'purple'}
    
    for policy, faults in results.items():
        plt.plot(memory_sizes, faults, label=policy, color=colors.get(policy, 'black'), marker='o')
    
    plt.xlabel("Number of Pages in Physical Memory")
    plt.ylabel("Number of Page Faults")
    plt.yscale('log')
    plt.title("Page Faults vs. Physical Memory Size for Various Policies")
    plt.legend()
    plt.grid(True, which="both", ls="--")
    plt.savefig(png_file)
    plt.show()

def plot_memory_access(page_access_list, png_file=None, instruction_page_set=None):

    # TODO: Implement (remove this comment before submission if you implemented somthing)

    return


if __name__ == "__main__":
    pages, instructions = get_page_list("test.log")
    print("Page Access List:", pages)
    print("Instruction Pages:", instructions)
    # testing the export_page_trace function
    new_list = export_page_trace(pages, "output.txt")
    # printing the ouput of export_page_trace
    print("New Page Access List:", new_list)
    # testing the simulate_page_replacement function
    memory_sizes, results = run_simulations(new_list, ['FIFO', 'LRU', 'RAND','OPT'])
    print("Simulation results:", results)
    plot_results(memory_sizes, results, "simresult.png")
    print("Plot created")


    


