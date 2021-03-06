// Copyright (c) 2007, Rodrigo Braz Monteiro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------
//
// AEGISUB
//
// Website: http://aegisub.cellosoft.com
// Contact: mailto:zeratul@cellosoft.com
//

#pragma once

///////////
// Headers
#include <wx/wxprec.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/combobox.h>
#include <map>


//////////////
// Prototypes
class SpellChecker;


//////////////////////////////
// Spell checker dialog class
class DialogSpellChecker : public wxDialog {
private:
	SpellChecker *spellchecker;

	std::map<wxString,wxString> autoReplace;
	wxArrayString autoIgnore;
	wxArrayString langCodes;

	int wordStart,wordEnd;
	int lastLine;
	int lastPos;
	int firstLine;

	wxTextCtrl *origWord;
	wxTextCtrl *replaceWord;
	wxListBox *suggestList;
	wxComboBox *language;
	wxButton *addButton;

	bool FindOrDie();
	bool FindNext(int startLine=-1,int startPos=-1);
	bool GetFirstMatch();
	void SetWord(wxString word);
	void Replace();

	void OnChangeLanguage(wxCommandEvent &event);
	void OnChangeSuggestion(wxCommandEvent &event);
	void OnTakeSuggestion(wxCommandEvent &event);

	void OnClose(wxCommandEvent &event);
	void OnReplace(wxCommandEvent &event);
	void OnReplaceAll(wxCommandEvent &event);
	void OnIgnore(wxCommandEvent &event);
	void OnIgnoreAll(wxCommandEvent &event);
	void OnAdd(wxCommandEvent &event);

public:
	DialogSpellChecker(wxFrame *parent);
	~DialogSpellChecker();

	DECLARE_EVENT_TABLE()
};
