"""Minimal MD3 reader/writer (single + multi frame)."""
import struct
class Surf: pass
class Model: pass
def load(path):
    d=open(path,'rb').read()
    m=Model()
    ident,ver,name,flags,nf,nt,ns,nsk,ofsf,ofst,ofss,ofse=struct.unpack('<4si64siiiiiiiii',d[:108])
    m.name=name.split(b'\0')[0]; m.flags=flags
    m.frames=[list(struct.unpack('<3f3f3ff16s',d[ofsf+i*56:ofsf+i*56+56])) for i in range(nf)]
    m.tags=[]  # per frame list of (name, origin[3], axis[3][3])
    for f in range(nf):
        fr=[]
        for i in range(nt):
            o=ofst+(f*nt+i)*112
            n=d[o:o+64].split(b'\0')[0].decode(); v=struct.unpack('<12f',d[o+64:o+112])
            fr.append([n,list(v[:3]),[list(v[3:6]),list(v[6:9]),list(v[9:12])]])
        m.tags.append(fr)
    m.surfs=[];o=ofss
    for i in range(ns):
        sid,sname,sfl,snf,nsh,nv,ntr,ofstri,ofssh,ofsst,ofsxyz,ofsend=struct.unpack('<4s64siiiiiiiiii',d[o:o+108])
        s=Surf(); s.name=sname.split(b'\0')[0].decode(); s.flags=sfl
        s.shaders=[(d[o+ofssh+k*68:o+ofssh+k*68+64].split(b'\0')[0].decode(), struct.unpack('<i',d[o+ofssh+k*68+64:o+ofssh+k*68+68])[0]) for k in range(nsh)]
        s.tris=[list(struct.unpack('<3i',d[o+ofstri+k*12:o+ofstri+k*12+12])) for k in range(ntr)]
        s.st=[list(struct.unpack('<2f',d[o+ofsst+k*8:o+ofsst+k*8+8])) for k in range(nv)]
        s.frames=[]
        for f in range(snf):
            base=o+ofsxyz+f*nv*8
            s.frames.append([list(struct.unpack('<3hH',d[base+k*8:base+k*8+8])) for k in range(nv)])
        m.surfs.append(s); o+=ofsend
    return m
def save(m,path):
    nf=len(m.frames); nt=len(m.tags[0]) if m.tags else 0; ns=len(m.surfs)
    out=bytearray(); ofsf=108; ofst=ofsf+56*nf; ofss=ofst+112*nf*nt
    fb=b''.join(struct.pack('<3f3f3ff16s',*f[:10],f[10]) for f in m.frames)
    tb=b''
    for fr in m.tags:
        for n,org,ax in fr:
            tb+=struct.pack('<64s12f',n.encode(),*org,*ax[0],*ax[1],*ax[2])
    sb=b''
    for s in m.surfs:
        nv=len(s.st); ntr=len(s.tris); nsh=len(s.shaders); snf=len(s.frames)
        ofstri=108; ofssh=ofstri+12*ntr; ofsst=ofssh+68*nsh; ofsxyz=ofsst+8*nv; ofsend=ofsxyz+8*nv*snf
        b=struct.pack('<4s64siiiiiiiiii',b'IDP3',s.name.encode(),s.flags,snf,nsh,nv,ntr,ofstri,ofssh,ofsst,ofsxyz,ofsend)
        b+=b''.join(struct.pack('<3i',*t) for t in s.tris)
        b+=b''.join(struct.pack('<64si',n.encode(),i) for n,i in s.shaders)
        b+=b''.join(struct.pack('<2f',*t) for t in s.st)
        for fr in s.frames: b+=b''.join(struct.pack('<3hH',*v) for v in fr)
        sb+=b
    ofse=ofss+len(sb)
    hdr=struct.pack('<4si64siiiiiiiii',b'IDP3',15,m.name.encode() if isinstance(m.name,str) else m.name,m.flags,nf,nt,ns,0,ofsf,ofst,ofss,ofse)
    open(path,'wb').write(hdr+fb+tb+sb)
import math
def dec_normal(n):
    lat=(n>>8)&255; lng=n&255
    lat*=2*math.pi/255; lng*=2*math.pi/255
    return (math.cos(lat)*math.sin(lng), math.sin(lat)*math.sin(lng), math.cos(lng))
def enc_normal(nx,ny,nz):
    l=math.sqrt(nx*nx+ny*ny+nz*nz) or 1; nx/=l;ny/=l;nz/=l
    if abs(nx)<1e-6 and abs(ny)<1e-6:
        return 0 if nz>0 else 128
    lng=int(math.acos(max(-1,min(1,nz)))*255/(2*math.pi))&255
    lat=int(math.atan2(ny,nx)*255/(2*math.pi))&255
    return (lat<<8)|lng
