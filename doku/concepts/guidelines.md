 
#code guidelines

* Push only clean code. The code should **not** cuase any (compiler) warnings or errors.
* Each file of ther kernel should contain the following GNU GPL license disclaimer
```
        Copyright 2012-2014 Infinitycoding all rights reserved
        This file is part of the Universe Kernel.
     
        The Universe Kernel is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        any later version.
 
        The Universe Kernel is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.
 
        You should have received a copy of the GNU General Public License
        along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
```

* And the files of the universe libraries should contain the following LGPL disclaimer.  
        (```<name>``` should be replaced with the library name)
        
```  
         Copyright 2012-2014 Infinitycoding all rights reserved
         This file is part of the <name>-library.
     
         The <name>-library is free software: you can redistribute it and/or modify
         it under the terms of the GNU Lesser General Public License as published by
         the Free Software Foundation, either version 3 of the License, or
         any later version.
     
         The <name>-library is distributed in the hope that it will be useful,
         but WITHOUT ANY WARRANTY; without even the implied warranty of
         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
         GNU Lesser General Public License for more details.
     
         You should have received a copy of the GNU Lesser General Public License
         along with the <name>-library.  If not, see <http://www.gnu.org/licenses/>.
```
* Files, functions and data structures should be documented with doxygen Tags:    
````
        /**
        * @file <file name>
        * @brief <brief description>
        * @author <file authors>
        * @todo <stuff to do>
        * @bug <bugs if reported>
        **/


        /**
         * @brief Returns the number of free pages
         * @return number of free pages
         */
        unsigned long pmm_count_free_pages()
        {
        ...
        }
````

 * Each module Has to be documented in a seperate file in the documentation folder.
   if the module is hardware dependent, the hardware has to discribed aswell.
