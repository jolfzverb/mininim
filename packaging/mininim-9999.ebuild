EAPI=5

inherit git-2
inherit eutils

DESCRIPTION="The Advanced Prince of Persia Engine"
HOMEPAGE="https://github.com/oitofelix/mininim"
EGIT_REPO_URI="https://github.com/oitofelix/mininim.git"

LICENSE="GPL"
SLOT="0"
IUSE=""
DEPEND="dev-libs/gnulib
>=media-libs/allegro-5.0.0[gtk,vorbis]"
RDEPEND="${DEPEND}"

src_prepare(){
  ./bootstrap
}

