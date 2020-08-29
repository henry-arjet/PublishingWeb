#pragma once
#include <tidy/tidy.h>
#include <tidy/tidybuffio.h>
#include <iostream>
#include <errno.h>
using std::string;

class Sanitizer {
public:
    void SanitizeHTML(const char* input) {
        SetInput(input);
        SetTrue(TidyXhtmlOut);
        Go();
        Finish();
    }
    string GetStr() {
        string ret = reinterpret_cast<const char*>(output.bp);
        tidyBufFree(&output);
        //cout << ret << endl;
        return ret;
    }
private:
    const char* input = nullptr;
    TidyDoc tdoc;
    int rc = -1;
    TidyBuffer errbuf = { 0 };
    TidyBuffer output = { 0 };

    void SetInput(const char* input) {
        this->input = input;
        tdoc = tidyCreate();
    }
    void SetTrue(TidyOptionId option) {
        tidyOptSetBool(tdoc, option, yes);
    }
    void SetFalse(TidyOptionId option) {
        tidyOptSetBool(tdoc, option, no);
    }
    void Go(){
        rc = tidySetErrorBuffer(tdoc, &errbuf);      // Capture diagnostics
        if (rc >= 0)
            rc = tidyParseString(tdoc, input);           // Parse the input
        if (rc >= 0)
            rc = tidyCleanAndRepair(tdoc);               // Tidy it up!
        if (rc >= 0)
            rc = tidyRunDiagnostics(tdoc);               // Kvetch
        if (rc > 1)                                    // If error, force output.
            rc = (tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1);
        if (rc >= 0)
            rc = tidySaveBuffer(tdoc, &output);          // Pretty Print
    }
    void Finish() {
        if (rc >= 0)
        {
            if (rc > 0)
                printf("\nDiagnostics:\n\n%s", errbuf.bp);
            printf("works");
        }
        else
            printf("A severe error (%d) occurred.\n", rc);

        tidyBufFree(&errbuf);
        tidyRelease(tdoc);
    }

    
	int DoStuff() {
        /*const char* input = "<title>Foo</title><p>Foo!";
        TidyBuffer output = { 0 };
        TidyBuffer errbuf = { 0 };
        int rc = -1;
        Bool ok;

        TidyDoc tdoc = tidyCreate();                     // Initialize "document"
        printf("Tidying:\t%s\n", input);

        ok = tidyOptSetBool(tdoc, TidyXhtmlOut, yes);  // Convert to XHTML
        if (ok)
            rc = tidySetErrorBuffer(tdoc, &errbuf);      // Capture diagnostics
        if (rc >= 0)
            rc = tidyParseString(tdoc, input);           // Parse the input
        if (rc >= 0)
            rc = tidyCleanAndRepair(tdoc);               // Tidy it up!
        if (rc >= 0)
            rc = tidyRunDiagnostics(tdoc);               // Kvetch
        if (rc > 1)                                    // If error, force output.
            rc = (tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1);
        if (rc >= 0)
            rc = tidySaveBuffer(tdoc, &output);          // Pretty Print

        if (rc >= 0)
        {
            if (rc > 0)
                printf("\nDiagnostics:\n\n%s", errbuf.bp);
            printf("\nAnd here is the result:\n\n%s", output.bp);
        }
        else
            printf("A severe error (%d) occurred.\n", rc);

        tidyBufFree(&output);
        tidyBufFree(&errbuf);
        tidyRelease(tdoc);
        return rc;*/
	}
};