--[[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is a bridge to the __SyncTeX__ package testing framework.

## License
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE
 
 Except as contained in this notice, the name of the copyright holder
 shall not be used in advertising or otherwise to promote the sale,
 use or other dealings in this Software without prior written
 authorization from the copyright holder.
 
--]]

local AUP = package.loaded.AUP

AUP.test_library_dump({'pdflatex', 'lualatex'}, 'form3',
--https://la-bibliotex.fr/2020/04/11/creer-des-pdf-editables-avec-latex/
[==[
\documentclass[11pt,a4paper]{article}

\usepackage[right=2cm,left=2cm,top=2cm,bottom=2cm]{geometry}
%\usepackage{luatextra}
\usepackage[french]{babel}
%\usepackage{fontspec}
%\setmainfont{Marianne}
\usepackage{xcolor}
\usepackage{hyperref}
  \hypersetup{
    colorlinks=true,
    linkcolor=black,
  }
\usepackage{fancyhdr}
\pagestyle{empty}

\setlength\parindent{0pt}

\begin{document}

\begin{Form}

\begin{center}
  \textbf{\uppercase{Attestation de déplacement dérogatoire}} \\
  En application de l'article 3 du décret du 23 mars 2020 prescrivant les mesures générales
  nécessaires pour faire face à l'épidémie de Covid19 dans le cadre de l'état d'urgence sanitaire 
\end{center}

Je soussigné(e), \\

\TextField[width=5cm,borderwidth=0pt]{Mme/M. :} \\
\TextField[width=5cm,borderwidth=0pt]{Né(e) le :} \\
\TextField[width=5cm,borderwidth=0pt]{À :} \\
\TextField[width=14cm,borderwidth=0pt]{Demeurant :} \\

certifie que mon déplacement est lié au motif suivant (cocher la case) autorisé par l'article 3
du décret du 23 mars 2020 prescrivant les mesures générales nécessaires pour faire face à
l'épidémie de Covid19 dans le cadre de l'état d'urgence sanitaire\footnote{Les personnes
souhaitant bénéficier de l'une de ces exceptions doivent se munir s'il y a lieu, lors de leurs
déplacements hors de leur domicile, d'un document leur permettant de justifier que le déplacement
considéré entre dans le champ de l'une de ces exceptions.} :\\

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Déplacements entre le domicile et le lieu d'exercice de l'activité professionnelle,
  lorsqu'ils sont indispensables à l'exercice d'activités ne pouvant être organisées sous
  forme de télétravail ou déplacements professionnels ne pouvant être différés\footnotemark.
\end{minipage}
\footnotetext{À utiliser par les travailleurs non-salariés, lorsqu'ils ne peuvent disposer d'un 
  justificatif de déplacement établi par leur employeur.}

\vspace{5mm}

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Déplacements pour effectuer des achats de fournitures nécessaires à l’activité
  professionnelle et des achats de première nécessité\footnotemark{} dans des établissements dont les
  activités demeurent autorisées (liste sur gouvernement.fr).
\end{minipage}
\footnotetext{Y compris les acquisitions à titre gratuit (distribution de denrées alimentaires\dots) et
  les déplacements liés à la perception de prestations sociales et au retrait d'espèces.}
  
\vspace{5mm}

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Consultations et soins ne pouvant être assurés à distance et ne pouvant être différés ;
  consultations et soins des patients atteints d'une affection de longue durée.
\end{minipage}

\vspace{5mm}

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Déplacements pour motif familial impérieux, pour l'assistance aux personnes
  vulnérables ou la garde d’enfants.
\end{minipage}

\vspace{5mm}

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Déplacements brefs, dans la limite d'une heure quotidienne et dans un rayon maximal
  d'un kilomètre autour du domicile, liés soit à l'activité physique individuelle des
  personnes, à l'exclusion de toute pratique sportive collective et de toute proximité avec
  d'autres personnes, soit à la promenade avec les seules personnes regroupées dans un
  même domicile, soit aux besoins des animaux de compagnie.
\end{minipage}

\vspace{5mm}

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Convocation judiciaire ou administrative.
\end{minipage}

\vspace{5mm}

\begin{minipage}{0.1\textwidth}
  \CheckBox[bordercolor=black]{}
\end{minipage}
\begin{minipage}{0.89\textwidth}
  Participation à des missions d'intérêt général sur demande de l'autorité administrative.
\end{minipage} \\

\TextField[width=8cm,borderwidth=0pt]{Fait à :} \\
\TextField[width=4cm,borderwidth=0pt]{Le :} 
\TextField[width=1cm,borderwidth=0pt,maxlen=2]{à} 
\TextField[width=1cm,borderwidth=0pt,maxlen=2]{h} \\
(Date et heure de début de sortie à mentionner obligatoirement) \\

\TextField[width=14cm,borderwidth=0pt]{Signature :} 

\end{Form}

\end{document}
]==])
