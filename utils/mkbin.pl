use warnings;
use strict;
use File::Basename;
use Cwd 'realpath';

my $COMBINED_HEX = "../build/app_combined.hex";
my $COMBINED_S_HEX = "../build/app_combined_s.hex";
my $COMBINED_S_BIN = "../build/app_combined_s.bin";

my $TRUNCATE_ADDRESS1 = "107F"; # small sector start address: 0x107F0000
my $TRUNCATE_ADDRESS2 = "1700"; # SFlash start address: 0x17000000

chdir realpath(dirname($0)) or die "cannot change dirctory..";

my $cy_tools_path = $ENV{CY_TOOLS_PATH};
if (!defined $cy_tools_path)
{
    my @path;
    my $win_home = $ENV{USERPROFILE};
    if (defined $win_home)
    {
        $win_home =~ s/\\/\//g;
        push(@path, glob($win_home."/ModusToolbox/tools_*"));
    }
    push(@path, glob($ENV{HOME}."/ModusToolbox/tools_*")) if (defined $ENV{HOME});
    push(@path, glob("/Applications/ModusToolbox/tools_*"));
    my $pathnum = @path;
    die "cannot find or create appropriate CY_TOOLS_PATH...\n" if ($pathnum == 0);
    @path = sort {$b cmp $a} @path;
    $cy_tools_path = $path[0];
}

my @objcopy;
push(@objcopy, glob($cy_tools_path."/gcc/arm-none-eabi/bin/objcopy*"));
push(@objcopy, glob($cy_tools_path."/gcc/arm-none-eabi-objcopy*"));
my $ocnum = @objcopy;
die "cannot find objcopy...\n" if ($ocnum == 0);

open my $rf, '<', $COMBINED_HEX or die "Could not open $COMBINED_HEX\n";
open my $wf, '>', $COMBINED_S_HEX or die "Could not open $COMBINED_S_HEX\n";
$TRUNCATE_ADDRESS1 = ":02000004".$TRUNCATE_ADDRESS1;
$TRUNCATE_ADDRESS2 = ":02000004".$TRUNCATE_ADDRESS2;
while (my $line = <$rf>)
{
    last if (($line =~ /$TRUNCATE_ADDRESS1/) || ($line =~ /$TRUNCATE_ADDRESS2/));
    print $wf $line;
}
close $rf or die;
close $wf or die;
my @result = `$objcopy[0] -v -I ihex -O binary $COMBINED_S_HEX $COMBINED_S_BIN`;
print foreach(@result);
