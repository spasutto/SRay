Version "boules de no�l": commit 29

Illumination Locale I:
======================
I=R+T
R: fraction r�fl�chie
T: fraction transmise
L'absorption n'est pas prise en compte.

R=Diffuse+Specular

Les r�fractions font parties de la fraction transmise T, le reste est dans la fraction r�fl�chie R.
R comprend donc un m�lange de r�flexion pure et de coloration locale au mat�riau compos�e elle m�me de r�fl�xion diffuse et de r�flexion speculaire. Le coefficient de sp�cularit� n'influe pas sur sur la partie sp�culaire de la coloration, seulement sur la fraction de lumi�re r�fl�chie venant du reste de la sc�ne.


==========
TODO:
==========
 - Normaliser le rayon � chaque it�ration (et non dans les fonctions intersect des Shape)




==========
NOTIONS
==========
ambient, diffus, sp�culaire
r�flexion sp�culaire: r�fl�xion dans une direction particuli�re (r�flexion "mirroir" par rapport � la normale)
r�flexion diffuse : r�flexion dans plusieurs directions � la fois
blinn, phong, lambert
diffuse interreflection (color bleeding, effet de lumi�re port�e color�e par r�flexion ou r�fraction sur un objet color�)
reflectance/transmittance (fraction d'�nergie incidente qui est r�fl�chit ou transmis � travers le materiaux). Pour un mat�riau, elles d�pendent de la longueur d'onde

Utah Teapot, Standford bunny, Cornell box

===========
Practical guide photon mapping
===========
diffuse surface? (p15)
diffuse point light (p15)
page 21 paragraphe 2: sens des �quations pour tenir compte de la probabilit� de survie
page 25: multiple scattering, anisotropic scattering

