import os
import sys
import importlib  

tracefile = "testinput.txt"
if len(sys.argv) > 1:
    tracefile = sys.argv[1]


sim = importlib.import_module("sim-paging")

# 
print("\nPart b/c: get_page_list(trace.txt)")
print(f"Using trace {tracefile}")
pagelist, iset = sim.get_page_list(tracefile)

print(f"Your function returns a pagelist with {len(pagelist)} pages")
print(f"Your function returns an instruction page set wit {len(iset)} pages")
#
if tracefile == "testinput.txt":
    print("pagelist: ", pagelist)
    print("iset: ", iset)
    print("Expected output for testinput.txt: pagelist: [16385,16385,16386,16386,16941,16940,33550336]    iset: {16385, 16386}")


print("\nPart d: export_page_trace")
print("Exporting to testoutput.txt")
sim.export_page_trace(pagelist, output_file="testoutput.txt")
if tracefile == "testinput.txt":
    print("File content:")
    os.system("cat testoutput.txt")

print("\nPart f (optional): plot_memory_access")
sim.plot_memory_access(pagelist, png_file="output.png")
print("Check the generated image in output.png")

