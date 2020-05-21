import numpy as np

htbin=[[250,450],[450,575],[575,1200],[1200,1500],[1500,np.Inf],[250,350],[350,450],[575,700],[700,1000],[1000,1200],[1200,np.Inf],[700,np.Inf]]
nJet30bin=[[2,4],[4,7],[7,10],[10,np.Inf],[1,2]]
nBJet20bin=[[0,1],[1,2],[2,3],[3,4],[4,np.Inf]]
selections=[]
for htrange in range(5):
    selections.append([htbin[htrange],nJet30bin[0],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[0],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[0],nBJet20bin[2]])
    selections.append([htbin[htrange],nJet30bin[1],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[1],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[1],nBJet20bin[2]])
    selections.append([htbin[htrange],[nJet30bin[0][0],nJet30bin[1][1]],[nBJet20bin[3][0],np.Inf]])
for htrange in [0,1]:
    selections.append([htbin[htrange],[nJet30bin[2][0],np.Inf],nBJet20bin[0]])
    selections.append([htbin[htrange],[nJet30bin[2][0],np.Inf],nBJet20bin[1]])
    selections.append([htbin[htrange],[nJet30bin[2][0],np.Inf],nBJet20bin[2]])
    selections.append([htbin[htrange],[nJet30bin[2][0],np.Inf],[nBJet20bin[3][0],np.Inf]])
for htrange in [2,3,4]:
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[2]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[3]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[4]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[2]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[3]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[4]])
selections.append([htbin[5],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[6],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[1],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[7],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[8],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[9],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[10],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[5],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[6],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[1],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[7],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[11],nJet30bin[4],nBJet20bin[1]])


selections_inclu=[]
'''
selections_inclu.append([[250,575],nJet30bin[0],nBJet20bin[0]])
selections_inclu.append([[250,575],nJet30bin[0],[1,3]])
selections_inclu.append([[575,1200],nJet30bin[0],nBJet20bin[0]])
selections_inclu.append([[1200,np.Inf],nJet30bin[0],nBJet20bin[0]])
selections_inclu.append([[575,1200],nJet30bin[0],[1,3]])
selections_inclu.append([[1200,np.Inf],nJet30bin[0],[1,3]])
selections_inclu.append([[250,575],nJet30bin[1],nBJet20bin[0]])
selections_inclu.append([[250,575],nJet30bin[1],[1,3]])
selections_inclu.append([[575,1200],nJet30bin[1],nBJet20bin[0]])
selections_inclu.append([[575,1200],nJet30bin[1],[1,3]])
selections_inclu.append([[1200,np.Inf],nJet30bin[1],nBJet20bin[0]])
selections_inclu.append([[1200,np.Inf],nJet30bin[1],[1,3]])
selections_inclu.append([[250,575],[nJet30bin[0][0],nJet30bin[1][1]],[nBJet20bin[3][0],np.Inf]])
selections_inclu.append([[575,1200],[nJet30bin[0][0],nJet30bin[1][1]],[nBJet20bin[3][0],np.Inf]])
selections_inclu.append([[1200,np.Inf],[nJet30bin[0][0],nJet30bin[1][1]],[nBJet20bin[3][0],np.Inf]])
selections_inclu.append([[250,575],[nJet30bin[2][0],np.Inf],nBJet20bin[0]])
selections_inclu.append([[575,1200],[nJet30bin[2][0],np.Inf],nBJet20bin[0]])
selections_inclu.append([[1200,np.Inf],[nJet30bin[2][0],np.Inf],nBJet20bin[0]])
selections_inclu.append([[250,575],[nJet30bin[2][0],np.Inf],[nBJet20bin[1][0],np.Inf]])
selections_inclu.append([[575,1200],[nJet30bin[2][0],np.Inf],[nBJet20bin[1][0],np.Inf]])
selections_inclu.append([[1200,np.Inf],[nJet30bin[2][0],np.Inf],[nBJet20bin[1][0],np.Inf]])
selections_inclu.append([htbin[5],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[6],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[1],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[7],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[8],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[9],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[10],nJet30bin[4],nBJet20bin[0]])
selections_inclu.append([htbin[5],nJet30bin[4],nBJet20bin[1]])
selections_inclu.append([htbin[6],nJet30bin[4],nBJet20bin[1]])
selections_inclu.append([htbin[1],nJet30bin[4],nBJet20bin[1]])
selections_inclu.append([htbin[7],nJet30bin[4],nBJet20bin[1]])
selections_inclu.append([htbin[11],nJet30bin[4],nBJet20bin[1]])
'''
selections_inclu.append([[250,np.Inf],[1,np.Inf],[0,np.Inf]])

def findinclu(binexclu):
    for bininclu,region in enumerate(selections_inclu):
        if selections[binexclu][0][0] >= region[0][0] \
           and selections[binexclu][0][1]<=region[0][1] \
           and selections[binexclu][1][0]>=region[1][0] \
           and selections[binexclu][1][1]<=region[1][1] \
           and selections[binexclu][2][0]>=region[2][0] \
           and selections[binexclu][2][1]<=region[2][1]:
            return bininclu

def findexclu(bininclu):
    binexclu_total=[]
    for binexclu,region in enumerate(selections):
        if region[0][0] >= selections_inclu[bininclu][0][0] \
           and region[0][1]<=selections_inclu[bininclu][0][1] \
           and region[1][0]>=selections_inclu[bininclu][1][0] \
           and region[1][1]<=selections_inclu[bininclu][1][1] \
           and region[2][0]>=selections_inclu[bininclu][2][0] \
           and region[2][1]<=selections_inclu[bininclu][2][1]:
            binexclu_total.append(binexclu)
    return binexclu_total
