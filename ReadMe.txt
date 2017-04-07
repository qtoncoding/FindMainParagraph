The solution is built in Visual Studio 2017, containing 3 project:
1.	FindMainParagraphLib: C++ class library, providing the processing API, returning OpenCV’s matrices.
2.	FindMainParagraphWinRT: C++/CX Windows Runtime Component library, a wrapper around the C++ library, providing the same API, returning Windows’ SoftwareBitmap.
3.	FindMainParagraph-Client: A Universal Windows application built with .NET, providing the user interface, OCR capabilities and consuming the WinRT library.

Additionally, there is a server application built with Django, providing the summarization service, optionally use Microsoft Cognitive Services for Spell Checking and Key-Phrase Detection. This service can be run locally and the client application can be configured to point to this local deployement.
To run server, run this command from Bash:
python3 manage.py runserver 0:8000