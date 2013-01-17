#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

// #define TIME

#ifdef TIME
    #include "pctimer.h"
#endif

using namespace llvm;

std::vector<std::string> includeStash;
std::vector<std::string> defineStash;
std::vector<std::string> replaceStash;

// Command Line Option Flags
// --------------------------
cl::opt<std::string>
InputFileName (cl::Positional, cl::desc("<input file>"));

cl::opt<bool>
License ("license", cl::desc("Print the license"));
cl::alias
LicenseA("l", cl::desc("Alias for -license"), cl::aliasopt(License), cl::Hidden);

cl::opt<std::string>
OutputFileName("o", cl::desc("Specify output filename"), cl::value_desc("filename"));

cl::list<std::string>
customIncludeFileLocationStash("i", cl::desc("Specify include file location"), cl::value_desc("path"));

cl::opt<bool>
Print("print", cl::desc("Print informational messages"));
cl::alias
PrintA("p", cl::desc("Alias for -print"), cl::aliasopt(Print));

cl::list<std::string>
Define("d", cl::desc("Specify a definition"), cl::value_desc("string,string"));

// Support procedures
// --------------------------
void printLicense()     /*{{{*/
{
    outs().changeColor(llvm::raw_ostream::RED);
    outs() <<
        " Copyright (c)2011-2013 John Kaul - john7kaul@gmail.com\n"
        " All rights reserved.\n"
        " \n"
        " Redistribution and use in source and binary forms, with or without\n"
        " modification, are permitted provided that the following conditions\n"
        " are met:\n"
        " \n"
        "  1. Redistributions of source code must retain the above copyright\n"
        "     notice, this list of conditions and the following disclaimer.\n"
        " \n"
        "  2. Redistributions in binary form must reproduce the above copyright\n"
        "     notice, this list of conditions and the following disclaimer in\n"
        "     the documentation and/or other materials provided with the\n"
        "     distribution.\n"
        " \n"
        " THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
        " \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
        " LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n"
        " FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE\n"
        " COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
        " INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n"
        " BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS\n"
        " OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED\n"
        " AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"
        " OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF\n"
        " THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH\n"
        " DAMAGE.\n";
    outs().resetColor();
}           /*}}}*/
const std::string trim(const std::string& pString, const std::string& pWhitespace = " \t") /*{{{*/
{
    const size_t beginStr = pString.find_first_not_of(pWhitespace);
    if (beginStr == std::string::npos)
        // no content
        return "";

    const size_t endStr = pString.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;

    return pString.substr(beginStr, range);
}
/*}}}*/
const std::string reduce(const std::string& pString, const std::string& pFill = " ", const std::string& pWhitespace = " \t") /*{{{*/
{
    // trim first
    std::string result(trim(pString, pWhitespace));

    // replace sub ranges
    size_t beginSpace = result.find_first_of(pWhitespace);
    while (beginSpace != std::string::npos)
    {
        const size_t endSpace =
            result.find_first_not_of(pWhitespace, beginSpace);
        const size_t range = endSpace - beginSpace;

        result.replace(beginSpace, range, pFill);

        const size_t newStart = beginSpace + pFill.length();
        beginSpace = result.find_first_of(pWhitespace, newStart);
    }
    return result;
}
/*}}}*/
std::string glue(std::vector<std::string> vect)     /*{{{*/
{
    int vectSize = vect.size();
    std::string gluestring;
    std::string space(" ");
    for(int i=0; i < vectSize; ++i)
    {
        gluestring.append(vect[i]);
        if (i != (vectSize))
            gluestring.append(space);
    }
    return gluestring;
}
/*}}}*/
std::vector<std::string> parse(std::string l, char delim)   /*{{{*/
// Example Ussage:
// int main()
// {
//    std::string str = ";#set __MYSET__ foo is a good solution";
//    std::vector<std::string> toks = parse(str, ' ');
//    for (int i=0; i < toks.size(); i++)
//        std::cout << "\nTOKS #" << i << ":" << toks[i] << '\n';
// }
{
    std::replace(l.begin(), l.end(), delim, ' ');
    std::istringstream stm(l);
    std::vector<std::string> tokens;
    for (;;) {
        std::string word;
        if (!(stm >> word)) break;
        tokens.push_back(word);
    }
    return tokens;
}
/*}}}*/
void ParseDefineArgument(std::string l)   /*{{{*/
{
    std::vector<std::string> tokens = parse(l, ',');
    if (tokens[0] == ";#define")
        tokens.erase(tokens.begin());
    // erase the ";#define" string if found
    if (tokens.size() > 1)
        replaceStash.push_back(glue(tokens));
    else
        defineStash.push_back(tokens[0]);
    //return tokens;
}
/*}}}*/
void ParseDefineArgument(std::vector<std::string> vl)   /*{{{*/
{
    std::vector<std::string> tokens;
    for (unsigned x = 0; x < vl.size(); ++x) {
        tokens = parse(vl[x], ',');
        if (tokens[0] == ";#define")
            tokens.erase(tokens.begin());
        // erase the ";#define" string if found
        if (tokens.size() > 1)
            replaceStash.push_back(glue(tokens));
        else
            defineStash.push_back(tokens[0]);
    }
}
/*}}}*/
bool OpenFileTest(std::string fileToTest )/*{{{*/
{
    bool result = false;
    raw_ostream *output = &outs();
    std::string ErrInfo;
    if (!(fileToTest.empty()))
        output = new raw_fd_ostream(fileToTest.c_str(), ErrInfo);
    if (output != &outs()) {
        result = true;
        delete output;
    }
    return result;
}/*}}}*/
void process(std::fstream& input, std::string& infilename, llvm::raw_ostream* output, std::string& outfilename )      /*{{{*/
{
    if (!input || !output)
    {
        if (!input) {
            outs().changeColor(llvm::raw_ostream::RED);
            errs() << "\nERROR: input file \"" << infilename << "\" not found or unable to open.\n"
                << "Output file creation has been halted.\n\n";
            outs().resetColor();
        }
        else if (!output) {
            outs().changeColor(llvm::raw_ostream::RED);
            errs() << "\nERROR: output file \"" << outfilename << "\" not found or unable to open.\n"
                   << "Output file creation has been halted.\n\n";
            outs().resetColor();
        }
        exit (1);
    }

    if (Print) {
        outs() << "\n[";
        outs().changeColor(llvm::raw_ostream::GREEN);
        outs() << "x";
        outs().resetColor();
        outs() << "]: " << infilename;
        // outs() << "\n[x]: " << infilename;
    }

    std::string str;
    size_t found;

    const std::string COMMENTSTRING     = ";@";
    // comment strings are ignored (skipped over) and not included in output file.
    const std::string INCLUDESTRING     = ";#include";
    // include strings are parsed and "followed" to include in output.
    const std::string IFDEFSTRING       = ";#ifdef";
    // used to have a block of code included or not based upon run time parameters
    const std::string IFNDEFSTRING      = ";#ifndef";
    // same as above but works in opposite manner
    const std::string ENDIFSTRING       = ";#endif";
    // used to close a IFDEF or IFNDEF block
    const std::string DEFINESTRING      = ";#define";
    // used to define parameter in source file (hardcoded) or used in a IFNDEF or IFDEF block.
    const std::string ERRORSTRING       = ";#error";
    // used to cancel processing in a IFNDEF block.
    const std::string KILLSTRING        = "!@#$%^&*()*&^%$#@!";

    while (getline(input, str))
    {
        // COMMENTS
        // := ;@ [string]
        size_t commentFound;
        commentFound = str.rfind(COMMENTSTRING);
        if (commentFound != std::string::npos)
            // if the comment string is found in the string
            str = KILLSTRING;
            // kill the string and move on.

        // =--------------------------------------------------=//
        //  include file parsing
        // =--------------------------------------------------=//
        found = str.rfind(INCLUDESTRING);
        if (found != std::string::npos)
        // if the INCLUDESTRING string is found in the string
        {
            std::string includeFile = str;
            // set up another string
            includeFile.replace(found, INCLUDESTRING.length(), "");

            includeFile = trim(includeFile);
            // strip the string of spaces.

            for (unsigned int i=0; i < customIncludeFileLocationStash.size(); ++i)
            {
                std::string testFile;
                testFile = includeFile;
                testFile.insert(0, customIncludeFileLocationStash[i]);
                if (OpenFileTest(testFile))
                {
                    includeFile = testFile;
                    break;

                }
            }

            bool proc = true;
            for (unsigned int i=0; i < includeStash.size(); i++)
                if (includeStash[i] == includeFile)
                    proc = false;

            if (proc)
            // if the "PROC" var is true then it has not been added to
            // the stash and needs to be processed.
            // otherwise we will just
            //
            {
                std::fstream include(includeFile.c_str(), std::fstream::in);
                // open the include file we found and
                process(include, includeFile, output, outfilename);
                // dig into the file to find any other include statements.
            }
            else
            // this section just reports that we are skipping a file, otherwise its not needed
            // if the -P option is removed from program.
            {
                if (Print)
                    outs() << "\n[ ]: " << includeFile.c_str();
            }

            str = KILLSTRING;
            // this line fixes the extraspace bug.
            // if the current line has an include statement for a file
            // we have already processed, repace the current line string
            // with this goofy string. Later we check to see if this goofy
            // string is present, if it is we dont write it out (skip it).
        }

        // =--------------------------------------------------=//
        //  preprocessor directives parsing
        // =--------------------------------------------------=//

        // IFDEF
        // := ;#ifdef [string]
        // := ;#ifdef _MYDIRECTIVE_
        size_t IFDEFfound;
        IFDEFfound = str.rfind(IFDEFSTRING);
        if (IFDEFfound !=  std::string::npos)
        {
            str.replace(IFDEFfound, IFDEFSTRING.length(), "");
            str = trim(str);
            // make sure the string is trimed of extra spaces

            bool proc = true;
            for (unsigned int i=0; i < defineStash.size(); i++)
                if (defineStash[i] == str)
                    proc = false;

            if (proc == true)
                // The definition has not been defined (added to the stash)
                // so we will eat up the lines until we find the ENDIF string.
                while (getline(input, str))
                {
                    str = trim(str);
                    if (str == ";#endif") break;
                }
            if (proc == false)
                // The definition has been defined (added to the stash) so we
                // can just eat this line and allow the code to keep adding
                // the following block of code within.
                str = KILLSTRING;
        }

        // IFNDEF
        // := ;#ifndef [string]
        // := ;#ifndef _MYDIRECTIVE_
        size_t IFNDEFfound;
        IFNDEFfound = str.rfind(IFNDEFSTRING);
        if (IFNDEFfound !=  std::string::npos)
        {
            str.replace(IFNDEFfound, IFNDEFSTRING.length(), "");
            str = trim(str);
            bool proc = true;
            // search the DEFINESTASH to see if definition has been added

            for (unsigned int i=0; i < defineStash.size(); i++)
                if (defineStash[i] == str)
                    proc = false;

            if (proc == false)
                // preprocessor def has been defined so we will
                // eat up the strings until we find the ENDIF string.
                while (getline(input, str))
                {
                    str = trim(str);
                    // strip the string of spaces. Since we are tossing out the strings
                    // anyways, we don't care to keep the formatting.
                    if (str == ";#endif") break;
                }
            if (proc == true)
                // preprocessor def has not been defined so we can just eat
                // this line and allow the code to keep adding the following
                // block.
                //
                str = KILLSTRING;
        }

         // ERROR
         // := ;#error [string]
         // := ;#error "_MYDIRECTIVE_" has not been defined
         size_t ERRORfound;
         ERRORfound = str.rfind(ERRORSTRING);
         if (ERRORfound !=  std::string::npos)
         {
             str.replace(ERRORfound, ERRORSTRING.length(), "");
             str = trim(str);
             // trim off extra spaces.

             outs().changeColor(llvm::raw_ostream::RED);
             errs() << "\nERROR: " << str;
             errs() << "\nAborting...";
             outs().resetColor();
             return;
         }

         // DEFINE
         //  := ;#define [string]
         //  := ;#define _MYDIRECTIVE_
         size_t DEFINEfound;
         DEFINEfound = str.rfind(DEFINESTRING);
         if (DEFINEfound !=  std::string::npos)
         {
             ParseDefineArgument(str);
             str = KILLSTRING;
             // eat the current line so it wont get added to output.
         }

         // REPLACE
         // This section should run if we need to search the strings for text
         // to replace.
         if (replaceStash.size() > 0 && str != KILLSTRING)
         {
             for (unsigned int i=0; i < replaceStash.size(); ++i)
             {
                 std::vector<std::string> toks = parse(replaceStash[i], ' ');
                 std::string SetNameToFind;
                 SetNameToFind = toks[0];

                 size_t SETDEFFound;
                 SETDEFFound = str.rfind(SetNameToFind);
                 if (SETDEFFound != std::string::npos)
                 {
                     toks.erase(toks.begin());
                     std::string replacementString;
                     replacementString = glue(toks);
                     replacementString = trim(replacementString);
                     str.replace(SETDEFFound, SetNameToFind.length(), replacementString);
                 }
             }
         }

        // ENDIF
        // := ;#endif
        //
        size_t ENDIFfound;
        ENDIFfound = str.rfind(ENDIFSTRING);
        if (ENDIFfound !=  std::string::npos)
            str = KILLSTRING;

        if (str != KILLSTRING && str != ";#endif")
            // finally, now that we are done checking for directives
            // we can write to the output our string.
            *output << str << '\n';
    }

    includeStash.push_back(infilename);
    // add "include file" to the included file stash so
    // duplicates are elimintated.
}
/*}}}*/

int main(int argc, char **argv)
{
    cl::ParseCommandLineOptions(argc, argv, " JASP (Just Another Simple Preprocessor)\n\n"
                                "  This program is a simple preprocessor for files \n"
                                "  which can be used to do simple substitutions in files.\n");
#ifdef TIME
    std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    pctimer_t t1, t2;
    t1 = pctimer();
#endif

    if (License) {
        printLicense();
        return 0;
    }

    if (!(Define.empty()))
        ParseDefineArgument(Define);

    if (!(InputFileName.empty())) {
        if (Print) {
            outs().changeColor(llvm::raw_ostream::YELLOW);
            outs() << "\nKey: " << "  [x] = Added" << "  [ ] = Skipped";
            outs().resetColor();
        }

        std::fstream input(InputFileName.c_str(), std::fstream::in);

        // set up output: use llvm raw_ostream
        raw_ostream *output = &outs();
        std::string ErrInfo;
        if (!(OutputFileName.empty()))
            output = new raw_fd_ostream(OutputFileName.c_str(), ErrInfo);

        process(input, InputFileName, output, OutputFileName);
        input.close();
        if (output != &outs())
            delete output;

        if (Print)
            outs() << '\n';
    }

#ifdef TIME
    std::cout.precision(7);
    t2 = pctimer();
    outs() << "This operation took: " << t2 - t1 << " seconds.\n\n";
#endif

    return 0;
}
