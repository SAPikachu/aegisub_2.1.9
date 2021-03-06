// Copyright (c) 2005, Rodrigo Braz Monteiro, Niels Martin Hansen
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


///////////
// Headers
#include "config.h"

#include <math.h>
#include <wx/tglbtn.h>
#include <wx/statline.h>
#include <wx/laywin.h>
#include <wx/recguard.h>
#include "audio_box.h"
#include "audio_display.h"
#include "audio_karaoke.h"
#include "frame_main.h"
#include "options.h"
#include "toggle_bitmap.h"
#include "hotkeys.h"
#include "tooltip_manager.h"

///////////////
// Constructor
///
AudioBox::AudioBox(wxWindow *parent) :
wxPanel(parent,-1,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL|wxBORDER_RAISED)
{
	// Setup
	loaded = false;
	karaokeMode = false;

	// Sash and Display
	audioScroll = new wxScrollBar(this,Audio_Scrollbar);
	audioScroll->PushEventHandler(new FocusEvent());
	audioScroll->SetToolTip(_("Seek bar"));
	Sash = new wxSashWindow(this,Audio_Sash,wxDefaultPosition,wxDefaultSize,wxCLIP_CHILDREN | wxSW_3DBORDER);
	sashSizer = new wxBoxSizer(wxVERTICAL);
	audioDisplay = new AudioDisplay(Sash);
	sashSizer->Add(audioDisplay,1,wxEXPAND,0);
	Sash->SetSizer(sashSizer);
	Sash->SetSashVisible(wxSASH_BOTTOM,true);
	//Sash->SetSashBorder(wxSASH_BOTTOM,true);
	Sash->SetMinimumSizeY(50);
	audioDisplay->ScrollBar = audioScroll;
	audioDisplay->box = this;
	int _w,_h;
	audioDisplay->GetSize(&_w,&_h);
	audioDisplay->SetSizeHints(-1,_h,-1,_h);

	// Zoom
	HorizontalZoom = new wxSlider(this,Audio_Horizontal_Zoom,50,0,100,wxDefaultPosition,wxSize(-1,20),wxSL_VERTICAL|wxSL_BOTH);
	HorizontalZoom->PushEventHandler(new FocusEvent());
	HorizontalZoom->SetToolTip(_("Horizontal zoom"));
	VerticalZoom = new wxSlider(this,Audio_Vertical_Zoom,50,0,100,wxDefaultPosition,wxSize(-1,20),wxSL_VERTICAL|wxSL_BOTH|wxSL_INVERSE);
	VerticalZoom->PushEventHandler(new FocusEvent());
	VerticalZoom->SetToolTip(_("Vertical zoom"));
	VolumeBar = new wxSlider(this,Audio_Volume,50,0,100,wxDefaultPosition,wxSize(-1,20),wxSL_VERTICAL|wxSL_BOTH|wxSL_INVERSE);
	VolumeBar->PushEventHandler(new FocusEvent());
	VolumeBar->SetToolTip(_("Audio Volume"));
	bool link = Options.AsBool(_T("Audio Link"));
	if (link) {
		VolumeBar->SetValue(VerticalZoom->GetValue());
		VolumeBar->Enable(false);
	}
	VerticalLink = new ToggleBitmap(this,Audio_Vertical_Link,wxBITMAP(toggle_audio_link));
	VerticalLink->SetToolTip(_("Link vertical zoom and volume sliders"));
	VerticalLink->SetValue(link);

	// Display sizer
	DisplaySizer = new wxBoxSizer(wxVERTICAL);
	//DisplaySizer->Add(audioDisplay,1,wxEXPAND,0);
	DisplaySizer->Add(Sash,0,wxEXPAND,0);
	DisplaySizer->Add(audioScroll,0,wxEXPAND,0);

	// VertVol sider
	wxSizer *VertVol = new wxBoxSizer(wxHORIZONTAL);
	VertVol->Add(VerticalZoom,1,wxEXPAND,0);
	VertVol->Add(VolumeBar,1,wxEXPAND,0);
	wxSizer *VertVolArea = new wxBoxSizer(wxVERTICAL);
	VertVolArea->Add(VertVol,1,wxEXPAND,0);
	VertVolArea->Add(VerticalLink,0,wxEXPAND,0);

	// Top sizer
	TopSizer = new wxBoxSizer(wxHORIZONTAL);
	TopSizer->Add(DisplaySizer,1,wxEXPAND,0);
	TopSizer->Add(HorizontalZoom,0,wxEXPAND,0);
	TopSizer->Add(VertVolArea,0,wxEXPAND,0);

	// Buttons sizer
	wxSizer *ButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *temp;
	temp = new wxBitmapButton(this,Audio_Button_Prev,wxBITMAP(button_prev),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Previous line or syllable (%KEY%/%KEY%)"),_T("Audio Prev Line"),_T("Audio Prev Line Alt"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Next,wxBITMAP(button_next),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Next line/syllable (%KEY%/%KEY%)"),_T("Audio Next Line"),_T("Audio Next Line Alt"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Play,wxBITMAP(button_playsel),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play selection (%KEY%/%KEY%)"),_T("Audio Play"),_T("Audio Play Alt"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Play_Row,wxBITMAP(button_playline),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play current line (%KEY%)"),_T("Audio Play Original Line"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Stop,wxBITMAP(button_stop),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Stop (%KEY%)"),_T("Audio Stop"));
	ButtonSizer->Add(temp,0,wxRIGHT,10);

	temp = new wxBitmapButton(this,Audio_Button_Play_500ms_Before,wxBITMAP(button_playfivehbefore),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play 500 ms before selection (%KEY%)"),_T("Audio Play 500ms Before"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Play_500ms_After,wxBITMAP(button_playfivehafter),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play 500 ms after selection (%KEY%)"),_T("Audio Play 500ms after"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Play_500ms_First,wxBITMAP(button_playfirstfiveh),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play first 500ms of selection (%KEY%)"),_T("Audio Play First 500ms"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Play_500ms_Last,wxBITMAP(button_playlastfiveh),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play last 500ms of selection (%KEY%)"),_T("Audio Play Last 500ms"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Play_To_End,wxBITMAP(button_playtoend),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Play from selection start to end of file (%KEY%)"),_T("Audio Play To End"));
	ButtonSizer->Add(temp,0,wxRIGHT,10);

	temp = new wxBitmapButton(this,Audio_Button_Leadin,wxBITMAP(button_leadin),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Add lead in (%KEY%)"),_T("Audio Add Lead In"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Leadout,wxBITMAP(button_leadout),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Add lead out (%KEY%)"),_T("Audio Add Lead Out"));
	ButtonSizer->Add(temp,0,wxRIGHT,10);

	temp = new wxBitmapButton(this,Audio_Button_Commit,wxBITMAP(button_audio_commit),wxDefaultPosition,wxSize(30,-1));
	ToolTipManager::Bind(temp,_("Commit changes (%KEY%/%KEY%)"),_T("Audio Commit (Stay)"),_T("Audio Commit Alt"));
	ButtonSizer->Add(temp,0,wxRIGHT,0);
	temp = new wxBitmapButton(this,Audio_Button_Goto,wxBITMAP(button_audio_goto),wxDefaultPosition,wxSize(30,-1));
	temp->SetToolTip(_("Go to selection"));
	ButtonSizer->Add(temp,0,wxRIGHT,10);

	AutoCommit = new ToggleBitmap(this,Audio_Check_AutoCommit,wxBITMAP(toggle_audio_autocommit),wxSize(30,-1));
	AutoCommit->SetToolTip(_("Automatically commit all changes"));
	AutoCommit->SetValue(Options.AsBool(_T("Audio Autocommit")));
	ButtonSizer->Add(AutoCommit,0,wxRIGHT | wxALIGN_CENTER | wxEXPAND,0);
	NextCommit = new ToggleBitmap(this,Audio_Check_NextCommit,wxBITMAP(toggle_audio_nextcommit),wxSize(30,-1));
	NextCommit->SetToolTip(_("Auto goes to next line on commit"));
	NextCommit->SetValue(Options.AsBool(_T("Audio Next Line on Commit")));
	ButtonSizer->Add(NextCommit,0,wxRIGHT | wxALIGN_CENTER | wxEXPAND,0);
	AutoScroll = new ToggleBitmap(this,Audio_Check_AutoGoto,wxBITMAP(toggle_audio_autoscroll),wxSize(30,-1));
	AutoScroll->SetToolTip(_("Auto scrolls audio display to selected line"));
	AutoScroll->SetValue(Options.AsBool(_T("Audio Autoscroll")));
	ButtonSizer->Add(AutoScroll,0,wxRIGHT | wxALIGN_CENTER | wxEXPAND,0);
	SpectrumMode = new ToggleBitmap(this,Audio_Check_Spectrum,wxBITMAP(toggle_audio_spectrum),wxSize(30,-1));
	SpectrumMode->SetToolTip(_("Spectrum analyzer mode"));
	SpectrumMode->SetValue(Options.AsBool(_T("Audio Spectrum")));
	ButtonSizer->Add(SpectrumMode,0,wxRIGHT | wxALIGN_CENTER | wxEXPAND,0);
	MedusaMode = new ToggleBitmap(this,Audio_Check_Medusa,wxBITMAP(toggle_audio_medusa),wxSize(30,-1));
	MedusaMode->SetToolTip(_("Enable Medusa-Style Timing Shortcuts"));
	MedusaMode->SetValue(Options.AsBool(_T("Audio Medusa Timing Hotkeys")));
	ButtonSizer->Add(MedusaMode,0,wxRIGHT | wxALIGN_CENTER | wxEXPAND,0);
	ButtonSizer->AddStretchSpacer(1);

	// Karaoke sizer
	wxSizer *karaokeSizer = new wxBoxSizer(wxHORIZONTAL);
	audioKaraoke = new AudioKaraoke(this);
	audioKaraoke->box = this;
	audioKaraoke->display = audioDisplay;
	audioDisplay->karaoke = audioKaraoke;
	KaraokeButton = new wxToggleButton(this,Audio_Button_Karaoke,_("Karaoke"),wxDefaultPosition,wxSize(-1,-1));
	KaraokeButton->SetToolTip(_("Toggle karaoke mode"));
	karaokeSizer->Add(KaraokeButton,0,wxRIGHT|wxEXPAND,0);
	JoinButton = new wxButton(this,Audio_Button_Join,_T(""),wxDefaultPosition,wxSize(-1,10));
	karaokeSizer->Add(JoinButton,0,wxRIGHT|wxEXPAND,0);
	SplitButton = new wxButton(this,Audio_Button_Split,_T(""),wxDefaultPosition,wxSize(-1,-1));
	karaokeSizer->Add(SplitButton,0,wxRIGHT|wxEXPAND,5);
	karaokeSizer->Add(audioKaraoke,1,wxEXPAND,0);
	SetKaraokeButtons(); // labels, hints and enabled-ness for join/split buttons set here

	// Main sizer
	MainSizer = new wxBoxSizer(wxVERTICAL);
	MainSizer->Add(TopSizer,0,wxEXPAND,0);
	MainSizer->Add(ButtonSizer,0,wxEXPAND,0);
	MainSizer->Add(new wxStaticLine(this),0,wxEXPAND|wxTOP|wxBOTTOM,2);
	MainSizer->Add(karaokeSizer,0,wxEXPAND,0);
	//MainSizer->SetSizeHints(this);
	SetSizer(MainSizer);
}


//////////////
// Destructor
AudioBox::~AudioBox() {
	audioScroll->PopEventHandler(true);
	HorizontalZoom->PopEventHandler(true);
	VerticalZoom->PopEventHandler(true);
	VolumeBar->PopEventHandler(true);
}


////////////
// Set file
void AudioBox::SetFile(wxString file,bool FromVideo) {
	loaded = false;

	if (FromVideo) {
		audioDisplay->SetFromVideo();
		loaded = audioDisplay->loaded;
		audioName = _T("?video");
	}

	else {
		audioDisplay->SetFile(file);
		if (file != _T("")) loaded = audioDisplay->loaded;
		audioName = file;
	}

	frameMain->SetAccelerators();
}


///////////////
// Event table
BEGIN_EVENT_TABLE(AudioBox,wxPanel)
	EVT_COMMAND_SCROLL(Audio_Scrollbar, AudioBox::OnScrollbar)
	EVT_COMMAND_SCROLL(Audio_Horizontal_Zoom, AudioBox::OnHorizontalZoom)
	EVT_COMMAND_SCROLL(Audio_Vertical_Zoom, AudioBox::OnVerticalZoom)
	EVT_COMMAND_SCROLL(Audio_Volume, AudioBox::OnVolume)
	EVT_SASH_DRAGGED(Audio_Sash,AudioBox::OnSash)

	EVT_BUTTON(Audio_Button_Play, AudioBox::OnPlaySelection)
	EVT_BUTTON(Audio_Button_Play_Row, AudioBox::OnPlayDialogue)
	EVT_BUTTON(Audio_Button_Stop, AudioBox::OnStop)
	EVT_BUTTON(Audio_Button_Next, AudioBox::OnNext)
	EVT_BUTTON(Audio_Button_Prev, AudioBox::OnPrev)
	EVT_BUTTON(Audio_Button_Play_500ms_Before, AudioBox::OnPlay500Before)
	EVT_BUTTON(Audio_Button_Play_500ms_After, AudioBox::OnPlay500After)
	EVT_BUTTON(Audio_Button_Play_500ms_First, AudioBox::OnPlay500First)
	EVT_BUTTON(Audio_Button_Play_500ms_Last, AudioBox::OnPlay500Last)
	EVT_BUTTON(Audio_Button_Play_To_End, AudioBox::OnPlayToEnd)
	EVT_BUTTON(Audio_Button_Commit, AudioBox::OnCommit)
	EVT_BUTTON(Audio_Button_Goto, AudioBox::OnGoto)
	EVT_BUTTON(Audio_Button_Join,AudioBox::OnJoin)
	EVT_BUTTON(Audio_Button_Split,AudioBox::OnSplit)
	EVT_BUTTON(Audio_Button_Leadin,AudioBox::OnLeadIn)
	EVT_BUTTON(Audio_Button_Leadout,AudioBox::OnLeadOut)

	EVT_TOGGLEBUTTON(Audio_Vertical_Link, AudioBox::OnVerticalLink)
	EVT_TOGGLEBUTTON(Audio_Button_Karaoke, AudioBox::OnKaraoke)
	EVT_TOGGLEBUTTON(Audio_Check_AutoGoto,AudioBox::OnAutoGoto)
	EVT_TOGGLEBUTTON(Audio_Check_Medusa,AudioBox::OnMedusaMode)
	EVT_TOGGLEBUTTON(Audio_Check_Spectrum,AudioBox::OnSpectrumMode)
	EVT_TOGGLEBUTTON(Audio_Check_AutoCommit,AudioBox::OnAutoCommit)
	EVT_TOGGLEBUTTON(Audio_Check_NextCommit,AudioBox::OnNextLineCommit)
END_EVENT_TABLE()


/////////////////////
// Scrollbar changed
void AudioBox::OnScrollbar(wxScrollEvent &event) {
	audioDisplay->SetPosition(event.GetPosition()*12);
}


///////////////////////////////
// Horizontal zoom bar changed
void AudioBox::OnHorizontalZoom(wxScrollEvent &event) {
	audioDisplay->SetSamplesPercent(event.GetPosition());
}


/////////////////////////////
// Vertical zoom bar changed
void AudioBox::OnVerticalZoom(wxScrollEvent &event) {
	int pos = event.GetPosition();
	if (pos < 1) pos = 1;
	if (pos > 100) pos = 100;
	float value = pow(float(pos)/50.0f,3);
	audioDisplay->SetScale(value);
	if (VerticalLink->GetValue()) {
		audioDisplay->player->SetVolume(value);
		VolumeBar->SetValue(pos);
	}
}


//////////////////////
// Volume bar changed
void AudioBox::OnVolume(wxScrollEvent &event) {
	if (!VerticalLink->GetValue()) {
		int pos = event.GetPosition();
		if (pos < 1) pos = 1;
		if (pos > 100) pos = 100;
		audioDisplay->player->SetVolume(pow(float(pos)/50.0f,3));
	}
}


////////////////////////
// Bars linked/unlinked
void AudioBox::OnVerticalLink(wxCommandEvent &event) {
	int pos = VerticalZoom->GetValue();
	if (pos < 1) pos = 1;
	if (pos > 100) pos = 100;
	float value = pow(float(pos)/50.0f,3);
	if (VerticalLink->GetValue()) {
		audioDisplay->player->SetVolume(value);
		VolumeBar->SetValue(pos);
	}
	VolumeBar->Enable(!VerticalLink->GetValue());

	Options.SetBool(_T("Audio Link"),VerticalLink->GetValue());
	Options.Save();
}


////////
// Sash
void AudioBox::OnSash(wxSashEvent& event) {
	// OK?
	if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE) return;

	// Recursion guard
	static wxRecursionGuardFlag inside;
	wxRecursionGuard guard(inside);
	if (guard.IsInside()) {
		return;
	}
	
	// Get size
	wxRect newSize = event.GetDragRect();
	int w = newSize.GetWidth();
	int h = newSize.GetHeight();
	if (h < 50) h = 50;
	int oldh = audioDisplay->GetSize().GetHeight();
	if (oldh == h) return;

	// Resize
	audioDisplay->SetSizeHints(w,h,-1,h);
	audioDisplay->SetSize(w,h);
	sashSizer->Layout();
	Sash->GetParent()->Layout();

	// Store new size
	Options.SetInt(_T("Audio Display Height"),h);
	Options.Save();

	// Fix layout
	frameMain->Freeze();
	DisplaySizer->Layout();
	//TopSizer->Layout();
	//MainSizer->Layout();
	Layout();
	frameMain->ToolSizer->Layout();
	frameMain->MainSizer->Layout();
	frameMain->Layout();
	frameMain->Refresh();
	frameMain->Thaw();

	//event.Skip();
}


//////////////////
// Play selection
void AudioBox::OnPlaySelection(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesSelection(start,end);
	audioDisplay->Play(start,end);
}


/////////////////
// Play dialogue
void AudioBox::OnPlayDialogue(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesDialogue(start,end);
	audioDisplay->SetSelection(start, end);
	audioDisplay->Play(start,end);
}


////////////////
// Stop Playing
void AudioBox::OnStop(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->Stop();
}


////////
// Next
void AudioBox::OnNext(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->Stop();
	audioDisplay->Next();
}


////////////
// Previous
void AudioBox::OnPrev(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->Stop();
	audioDisplay->Prev();
}


/////////////////
// 500 ms before
void AudioBox::OnPlay500Before(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesSelection(start,end);
	audioDisplay->Play(start-500,start);
}


////////////////
// 500 ms after
void AudioBox::OnPlay500After(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesSelection(start,end);
	audioDisplay->Play(end,end+500);
}


////////////////
// First 500 ms
void AudioBox::OnPlay500First(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesSelection(start,end);
	int endp = start+500;
	if (endp > end) endp = end;
	audioDisplay->Play(start,endp);
}


///////////////
// Last 500 ms
void AudioBox::OnPlay500Last(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesSelection(start,end);
	int startp = end-500;
	if (startp < start) startp = start;
	audioDisplay->Play(startp,end);
}


////////////////////////
// Start to end of file
void AudioBox::OnPlayToEnd(wxCommandEvent &event) {
	int start=0,end=0;
	audioDisplay->SetFocus();
	audioDisplay->GetTimesSelection(start,end);
	audioDisplay->Play(start,-1);
}


//////////////////
// Commit changes
void AudioBox::OnCommit(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->CommitChanges(true);
}


//////////////////
// Toggle karaoke
void AudioBox::OnKaraoke(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	if (karaokeMode) {
		if (audioKaraoke->splitting) {
			audioKaraoke->EndSplit(false);
		}
		karaokeMode = false;
		audioKaraoke->enabled = false;
		audioDisplay->SetDialogue();
		audioKaraoke->Refresh(false);
	}

	else {
		karaokeMode = true;
		audioKaraoke->enabled = true;
		audioDisplay->SetDialogue();
	}
	SetKaraokeButtons();

}


////////////////////////
// Sets karaoke buttons
void AudioBox::SetKaraokeButtons() {
	// What to enable
	bool join,split;
	join = audioKaraoke->enabled && (audioKaraoke->splitting || audioKaraoke->selectionCount>=2);
	split = audioKaraoke->enabled;

	// If we set focus here, the audio display will continually steal the focus
	// when navigating via the grid and karaoke is enabled. So don't.
	//audioDisplay->SetFocus();

	JoinButton->Enable(join);
	SplitButton->Enable(split);
	if (audioKaraoke->splitting) {
		JoinButton->SetLabel(_("Cancel Split"));
		JoinButton->SetToolTip(_("Discard all splits and leave split-mode"));
		SplitButton->SetLabel(_("Accept Split"));
		SplitButton->SetToolTip(_("Commit splits and leave split-mode"));
	} else {
		JoinButton->SetLabel(_("Join"));
		JoinButton->SetToolTip(_("Join selected syllables"));
		SplitButton->SetLabel(_("Split"));
		SplitButton->SetToolTip(_("Enter split-mode"));
	}
}


///////////////
// Join button
void AudioBox::OnJoin(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	if (!audioKaraoke->splitting) {
		audioKaraoke->Join();
	} else {
		audioKaraoke->EndSplit(false);
	}
}


////////////////
// Split button
void AudioBox::OnSplit(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	if (!audioKaraoke->splitting) {
		audioKaraoke->BeginSplit();
	} else {
		audioKaraoke->EndSplit(true);
	}
}


///////////////
// Goto button
void AudioBox::OnGoto(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->MakeDialogueVisible(true);
}


/////////////
// Auto Goto
void AudioBox::OnAutoGoto(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	Options.SetBool(_T("Audio Autoscroll"),AutoScroll->GetValue());
	Options.Save();
}


///////////////
// Auto Commit
void AudioBox::OnAutoCommit(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	Options.SetBool(_T("Audio Autocommit"),AutoCommit->GetValue());
	Options.Save();
}


//////////////////////
// Next line on Commit
void AudioBox::OnNextLineCommit(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	Options.SetBool(_T("Audio Next Line on Commit"),NextCommit->GetValue());
	Options.Save();
}


///////////////
// Medusa Mode
void AudioBox::OnMedusaMode(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	Options.SetBool(_T("Audio Medusa Timing Hotkeys"),MedusaMode->GetValue());
	Options.Save();
	frameMain->SetAccelerators();
}


//////////////////////////
// Spectrum Analyzer Mode
void AudioBox::OnSpectrumMode(wxCommandEvent &event) {
	Options.SetBool(_T("Audio Spectrum"),SpectrumMode->GetValue());
	Options.Save();
	audioDisplay->UpdateImage(false);
	audioDisplay->SetFocus();
	audioDisplay->Refresh(false);
}


///////////////
// Lead in/out
void AudioBox::OnLeadIn(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->AddLead(true,false);
}

void AudioBox::OnLeadOut(wxCommandEvent &event) {
	audioDisplay->SetFocus();
	audioDisplay->AddLead(false,true);
}


//////////////////////////////////////////
// Focus event handling for the scrollbar
BEGIN_EVENT_TABLE(FocusEvent,wxEvtHandler)
	EVT_SET_FOCUS(FocusEvent::OnSetFocus)
END_EVENT_TABLE()

void FocusEvent::OnSetFocus(wxFocusEvent &event) {
	wxWindow *previous = event.GetWindow();
	if (previous) previous->SetFocus();
}


