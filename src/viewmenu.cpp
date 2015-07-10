#include "base.h"

#include "enums.h"
#include "inputdialog.h"
#include "tools.h"
#include <wx/colordlg.h>
#include <wx/fileconf.h>
#include <wx/filedlg.h>
#include <wx/menu.h>

void MainFrame::OnSaveChat(wxCommandEvent & WXUNUSED(event)) {
	wxFileDialog *tmp=new wxFileDialog(this, langIni->Read(wxT("translations/menus/view/savechathistory"), wxT("Save chat...")), wxEmptyString, wxT("moos.htm"), langIni->Read(wxT("translations/other/htmlfiles"), wxT("HTML Files"))+wxT(" (*.htm;*.html)|*.htm;*.html|")+langIni->Read(wxT("translations/other/allfiles"), wxT("All Files"))+wxT(" (*.*)|*.*"), wxFD_SAVE);
	if (tmp->ShowModal()==wxID_OK) {
		if ((tmp->GetPath().SubString(tmp->GetPath().Length()-4, tmp->GetPath().Length())==wxT(".htm")) || (tmp->GetPath().SubString(tmp->GetPath().Length()-5, tmp->GetPath().Length())==wxT(".html"))) {
			wxFile file(tmp->GetPath(), wxFile::write);
			file.Write("\xFF\xFE", 2); //UTF-16LE BOM
			file.Write((chatHistory+wxT("</pre></body></html>")).c_str(), chatHistory.Len()*2+40);
			file.Close();
		} else {
			chatView->SaveFile(tmp->GetPath());
		}
	}
	chatInput->SetFocus();
}

void MainFrame::OnFreezeChat(wxCommandEvent & WXUNUSED(event)) {
	if (viewMenu->IsChecked(ID_MAINWIN_FREEZE_CHAT)) {
		return;
	}
	for (size_t i=0;i!=chatBuffer.GetCount();i++) {
		Message(chatBuffer[i]);
	}
	chatBuffer.Clear();
}

void MainFrame::OnDeleteChat(wxCommandEvent & WXUNUSED(event)) {
	chatView->Clear();
	chatHistory=wxT("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head><meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-16LE\"><title>moos2.1</title><style type=\"text/css\">body{background-color:#1F3038; color:#8C9FBB}pre{font-family:\"ms sans serif\", sans-serif;}</style></head><body><pre>");
	chatInput->SetFocus();
}

void MainFrame::OnColorSelect(wxCommandEvent & WXUNUSED(event)) {
	if (loginName==wxEmptyString) return;
	wxColourData tmp;
	tmp.SetChooseFull(true);
	tmp.SetCustomColour(0, Long2Color(moosIni->Read(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"),
									  langIni->Read(wxT("translations/channel/defaultcolor"), 0xFFBB9F8C))));
	tmp.SetColour(Long2Color(moosIni->Read(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"),
										   langIni->Read(wxT("translations/channel/defaultcolor"), 0xFFBB9F8C))));
	wxColourDialog tmp2(this, &tmp);
	if (tmp2.ShowModal()!=wxID_OK || Color2Long(tmp2.GetColourData().GetColour())==langIni->Read(wxT("translations/channel/defaultcolor"), 0xFFBB9F8C)) return;
	moosIni->Write(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"), Color2Long(tmp2.GetColourData().GetColour()));
	chatInput->SetFocus();
}

void MainFrame::OnManualColorCode(wxCommandEvent & WXUNUSED(event)) {
	if (loginName==wxEmptyString) return;
	InputDialog *tmp;
	if (moosIni->Read(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"), langIni->Read(wxT("translations/channel/defaultcolor"), 0xFFBB9F8C))
			!=langIni->Read(wxT("translations/channel/defaultcolor"), 0xFFBB9F8C)) {
		tmp=new InputDialog(this, langIni, font.GetChosenFont(),
							langIni->Read(wxT("translations/menus/view/manualcolorcode"), wxT("Manual Color...")),
							langIni->Read(wxT("translations/dialogtext/entercolorcode"), wxT("Please enter Colorcode")), 0,
							Long2MoonCode(moosIni->Read(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"), 0xFFBB9F8C)));
	} else {
		tmp=new InputDialog(this, langIni, font.GetChosenFont(),
							langIni->Read(wxT("translations/menus/view/manualcolorcode"), wxT("Manual Color...")),
							langIni->Read(wxT("translations/dialogtext/entercolorcode"), wxT("Please enter Colorcode")));
	}
	if (tmp->ShowModal()!=wxID_OK) return;
	if (tmp->GetValue()==wxEmptyString) moosIni->DeleteEntry(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"));
	if (tmp->GetValue().Len()!=12 || tmp->GetValue()[0]!='<' || tmp->GetValue()[1]!='0'
			|| !(tmp->GetValue()[2]=='x' || tmp->GetValue()[2]=='X')
			|| !IsHex(tmp->GetValue().Mid(3, 8)) || tmp->GetValue()[11]!='>') return;
	long tmp2=MoonCode2Long(tmp->GetValue());
	if (tmp2==langIni->Read(wxT("translations/channel/defaultcolor"), 0xFFBB9F8C)) return;
	moosIni->Write(wxT("accounts/")+Base64Encode(loginName)+wxT("/chatcolor"), tmp2);
	chatInput->SetFocus();
}