#include "MainWindowAboutDialog.h"

MainWindowAboutDialog::MainWindowAboutDialog(wxWindow *parent)
    : AboutDialog(parent)
{
}

void MainWindowAboutDialog::m_aboutOnHtmlLinkClicked(wxHtmlLinkEvent &event)
{

    wxString url = event.GetLinkInfo().GetHref();
    wxLaunchDefaultBrowser(url);
}
