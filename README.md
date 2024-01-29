# eBPF-MojitOS
PLugin to implement EBPF on 


pour le programme "packet_compteur" , il faut attacher le programme à la mano :


-  sudo bpftool prog load packet_compteur.bpf.o /sys/fs/bpf/packet_compteur : charger le programme dans le kernel 

-  ls /sys/fs/bpf/ pour vérifier que le prog soit bien chargé

-  sudo bpftool prog list : trouver l'id du programme

- sudo bpftool net attach xdp id [id value] dev "nom_interface_réseaux" : attache le programme à une interface réseau

- sudo bpftool net detach xdp dev "nom_interface_réseaux" : pour détacher le prog de l'interface réseau 

- sudo rm /sys/fs/bpf/packet_compteur : retirer le programme du kernel 



( pour regarder l'output du prog ====> sudo cat /sys/kernel/debug/tracing/trace_pipe )



Pour le programme packet_compteur_egress il suffit de faire:

- make clean puis make 

- sudo ./packet_compteur et hop ça affiche le résultat directement sur le terminal (à chaque seconde)


( vérifier le nom de l'interface réseau dans le programme packet_compteur.c, mettre une interface qui existe sur votre pc à la place)