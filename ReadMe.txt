Version "boules de noël": commit 29

Illumination Locale I:
======================
I=R+T
R: fraction réfléchie
T: fraction transmise
L'absorption n'est pas prise en compte.

R=Diffuse+Specular

Les réfractions font parties de la fraction transmise T, le reste est dans la fraction réfléchie R.
R comprend donc un mélange de réflexion pure et de coloration locale au matériau composée elle même de réfléxion diffuse et de réflexion speculaire. Le coefficient de spécularité n'influe pas sur sur la partie spéculaire de la coloration, seulement sur la fraction de lumière réfléchie venant du reste de la scène.


==========
TODO:
==========
 - Normaliser le rayon à chaque itération (et non dans les fonctions intersect des Shape)




==========
NOTIONS
==========
ambient, diffus, spéculaire
réflexion spéculaire: réfléxion dans une direction particulière (réflexion "mirroir" par rapport à la normale)
réflexion diffuse : réflexion dans plusieurs directions à la fois
blinn, phong, lambert
diffuse interreflection (color bleeding, effet de lumière portée colorée par réflexion ou réfraction sur un objet coloré)
reflectance/transmittance (fraction d'énergie incidente qui est réfléchit ou transmis à travers le materiaux). Pour un matériau, elles dépendent de la longueur d'onde

Utah Teapot, Standford bunny, Cornell box

===========
Practical guide photon mapping
===========
diffuse surface? (p15)
diffuse point light (p15)
page 21 paragraphe 2: sens des équations pour tenir compte de la probabilité de survie
page 25: multiple scattering, anisotropic scattering

