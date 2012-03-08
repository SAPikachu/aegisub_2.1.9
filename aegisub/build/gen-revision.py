import subprocess
import os
import re
import sys

def generate_revision_file(src_dir, template_file, output_file, git_exec):
    curdir = os.path.abspath(os.curdir)
    os.chdir(src_dir)
    info = subprocess.check_output([git_exec, "svn", "info"]).decode("utf-8")
    os.chdir(curdir)
    rev = re.search(r"Revision: (\d+)", info).group(1)

    date = subprocess.check_output(
        [git_exec, "show", "-s", "--format=%ci"]
    ).decode("utf-8").strip()

    base_rev = subprocess.check_output(
        [git_exec, "merge-base", "HEAD", "git-svn"]
    ).decode("utf-8").strip()

    local_revs = subprocess.check_output(
        [git_exec, "log", "--pretty=oneline", base_rev + ".."]
    ).decode("utf-8")

    local_rev_count = len([x for x in local_revs.splitlines() if x.strip()])

    with open(template_file, "r") as f:
        rev_file = f.read()

    rev_file = rev_file.replace("$WCREV$", rev)
    rev_file = rev_file.replace("$WCDATE$", date)
    rev_file = rev_file.replace("$WCMODS?true:false$", "true")

    with open(output_file, "w") as f:
        f.write(rev_file)
        f.write("\n")
        f.write("#define BUILD_LOCAL_REVISION " + str(local_rev_count))

if __name__ == "__main__":
    generate_revision_file(*(sys.argv[1:]))
