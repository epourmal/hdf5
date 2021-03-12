# How to contribute to HDF5 (Draft)

The HDF Group encourages community members to contribute to the HDF5 project. We accept and are very grateful for any type of contributions 
from small typos and bug fixes to new features. The HDF Group is committed to working with code contributors to make the contribution
process simple and productive.

We assume contributors are familiar with [`git`](https://git-scm.com) and [`GitHub`](https://github.com) [pull requests](https://guides.github.com/activities/hello-world/).
Except in special circumstances, [large contributions](https://bssw.io/items/pull-request-size-matters)
(those involving changes to more than 500 lines of code and/or 50 source files) will in all likelihood be required to be
split across multiple PRs.

This document describes overall guiding principles for HDF5 code contributions and does not pretend to address any specific contribution.
If in doubt, please engage us for guidance *ahead* of submitting a pull request by filing a [GitHub issue](https://github.com/HDFGroup/hdf5/issues/new). 
**Note that contributors must agree to HDF Group software license terms found in the
[`COPYING`](https://github.com/HDFGroup/hdf5/blob/develop/COPYING) file in the top directory of every branch before
contributions can be considered for acceptance.**

## Table of Contents

* [Workflow](#workflow)
* [Acceptance criteria](#acceptance-criteria)
* [Check List](#checklist)

## Workflow

The process for contributing code to HDF5 is as follows:

* Open an issue on [HDF5 GitHub](https://github.com/HDFGroup/hdf5/issues).
  * This step is **required** unless the change is *very* minor (e.g., typo fix).
  * Please follow a template found in [issue_template.md]() file if possible.  
* Fork the [HDF5](https://github.com/HDFGroup/hdf5) repository.
  * Use either `git pull upstream <branchname>` or `git fetch upstream <branchname>` followed by `git merge upstream/<branchname>`
    to keep your fork up to date with changes occuring upstream.
* Make the desired changes to the HDF5 software.
  * New features should always go to develop branch first and later should be merged to the appropriate maintenance branches.
  * Bug fixes should go to all appropriate branches (develop and maintenance). 
* Build and test your changes. Detailed instructions on how to build and test HDF5 can be found in the
  [`INSTALL*`](https://github.com/HDFGroup/hdf5/search?q=filename%3AINSTALL_) files.
* Push your changes to GitHub being sure to `git add` any new source files you've added.
* Create a pull request and monitor its CI *status*. Address any code formatting and testing issues CI reports.
  * Be aware that it may take as long as week before any developers are able to review your PR.
  * Remember, [smaller PRs](https://bssw.io/items/pull-request-size-matters) are far more likely to be reviewed quickly.

Only *after* a pull request is correctly formatted and passes **ALL** CI tests, will it be reviewed and evaluated by The
HDF Group developers and HDF5 community members who can approve pull requests.
The HDF Group developers will work with you to assure that the pull request satisfies acceptance criteria described in the next section. 

## Acceptance criteria

While we appreciate every contribution we receive, we may not be able accept all contributions.
Those that we *do* accept satisfy the following criteria:

### **The pull request has a clear, well constrained scope and purpose**
* What issue or feature does the pull request address?
* How does it benefit the HDF5 community?
* What are some example workflow(s) that the changes help?
* **Note::** For large changes, creating multiple PRs that each tackle a specific part and
  where each part provides value-added functionality on its own (apart from any other part) is a *best practice*.

### **The pull request is documented**
HDF5 developers need to be able to understand not only *what* a change is doing, but *how* it is doing it.
Documentation needed as part of a pull request may involve one or more of the following
* Comments in the GitHub *conversation* for the pull request itself. For example, the initial comment in the
  PR should address the question raised above regarding scope and purpose of the PR.
* In-line comments in the source code that explain any portion of code that would not be immediatly obvious from the
  code itself. For example, in the code block below, lines bracketed with `>>` and `<<` represent **unnecessary**
  comments because the code itself sufficiently explains what it is doing whereas other comments are *necessary* 
  because they help to explain non-obviouse behavior.
  ```
  /*
  >> Function to compute total number of datums *currently* in a dataset <<
    >> dsid: the input dataset id <<
    returns: >= 0 on success or negative on failure
  */
  int get_dataset_size_in_datums(hid_t dsid)
  {
      >> /* lookup the dataset handle from the id */ << 
      get_dataset_handle(dsid);
      
      >> /* check of dataset is extendable */ <<
      is_ext = is_dataset_extendable(dsid);

      /* If dataset is extendible, we may need to do actual I/O to obtain *current* size */
      if (is_ext)
          ds_eol = read_dataset_eol(dsid);
  }
  ```
  * Updating design and/or implementation documents
  * Updating reference manual
  * Updating release notes

### **The pull request is tested**
Any issue fixed or functionality added should be accompanied by corresponding tests. In addition, existing HDF5 tests must pass.
Code coverage should not *decrease* in a PR. We do not expect contributors to perform comprehensive testing across all possible
platforms before a PR is accepted. However, if the PR does not pass regression testing after the merge, THG developers will need
to work with contributors to get the code fixed. If contributors fail to be responsive during this process, in all likelihood
the changes will have to be backed out until such time as the contributor is able to respond to all regression testing issues.

### **The pull request is consistent with HDF5 design and architecture**
* HDF5 has a 100% commitment to backward compatibility of data files.
  * Any file ever created with HDF5 must be readable by any future version of HDF5. If you expect your contribution to include
    changes to the HDF5 data model or file format, **please** engage with us *before* starting work by filing a
    [GitHub issue](https://github.com/HDFGroup/hdf5/issues/new). In addition, file format changes and features needing backward
    compatibility support can be introduced only in a new *major* release.
* HDF5 has a committment to backward compatability of its API. This includes Application Binary Interface (ABI) across minor/patch
  releases as well as the Application Programming Interface.
* HDF5 has a commitment to ensuring HDF5 data remains *machine-independent*.
  * Data created on one platform/environment/architecture/VFD/VOL **must** remain readable by HDF5 on any other. 
  * For binary compatibility no changes are allowed to public APIs and data structures in the maintenance releases; new APIs can be added.

### **New features are documented**
Any new features should have proper documentation; talk to us if you have any questions.

# Checklist

Please make sure that you check the items applicable to your pull request:

* Code 
  * [ ] Does the pull request have a coresponding GitHub issue and clear purpose?
  * [ ] Does the pull request follow HDF5 best practices (naming convensions, code portability, code structure, etc.)? <<TODO: link to the document>>
  * [ ] If changes were done to autotools build were they added to CMake and vice versa??
  * [ ] Is the pull request applicable to any other branches? If yes, which ones? Please document it in the GitHub issue.
  * [ ] Is the new code sufficiently documented for future maintenance?
* Documentation
  * [ ] Was the change described in the release_docs/RELEASE.txt file?
  * [ ] Was MANIFEST updated if new files had been added to the source?
  * [ ] Was new function documented in the corresponding public header file using Doxygen? <<TODO: link to Doxygen instructions>>
  * [ ] Was new functionality documented for the HDF5 community (the level of documentation depends on the feature; ask us what would be appropriate)
* Testing
  * [ ] Does the pull request have tests?
  * [ ] If a new feature is added, does it affect HDF5 library perfromance?
  * [ ] Does new feature introduce backward or forward incompatibility? <<TODO: link to the document>>

We want as many contributions as we can get, and we are here to help. Feel free to reach out to us if you have any questions

Thank you for your contribution!

